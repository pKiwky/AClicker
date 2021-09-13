#include <AClicker.h>
#include <ASettings.h>

#define CLAMP_TO_MIN(number,minimum) number < minimum ? minimum : number
#define TOGGLE_CLICKER(ui) \
	g_Settings->m_IsRunning = !g_Settings->m_IsRunning; \
	QMetaObject::invokeMethod(ui->pushButtonStart, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, !g_Settings->m_IsRunning)); \
	QMetaObject::invokeMethod(ui->pushButtonStop, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, g_Settings->m_IsRunning));

extern Settings *g_Settings;

AClicker::AClicker(QWidget *parent): QMainWindow(parent), m_Interface(new Ui::AClicker), m_Thread(nullptr) {
	m_Interface->setupUi(this);
	g_Settings = new Settings();

	setWindowTitle("AClicker V1.4");
	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

	connect(m_Interface->pushButtonStart, SIGNAL(pressed()), this, SLOT(OnStartPressed()));
	connect(m_Interface->pushButtonStop, SIGNAL(pressed()), this, SLOT(OnStopPressed()));
	connect(m_Interface->pushButtonSettings, SIGNAL(pressed()), this, SLOT(OnSettingsPressed()));
	connect(m_Interface->lineEditMinutes, SIGNAL(editingFinished()), this, SLOT(OnMinutesEdited()));
	connect(m_Interface->lineEditSeconds, SIGNAL(editingFinished()), this, SLOT(OnSecondsEdited()));
	connect(m_Interface->lineEditMilliseconds, SIGNAL(editingFinished()), this, SLOT(OnMillisecondsEdited()));
	connect(m_Interface->lineEditRepeatTimes, SIGNAL(editingFinished()), this, SLOT(OnClickCountChanged()));
	connect(m_Interface->radioButtonRepeatInf, SIGNAL(pressed()), this, SLOT(OnClickCountInfinitePressed()));
	connect(m_Interface->comboBoxClickButton, SIGNAL(currentIndexChanged(int)), this, SLOT(OnClickButtonChanged(int)));
	connect(m_Interface->comboBoxClickType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnClickTypeChanged(int)));

	m_Interface->comboBoxClickButton->addItem("Left Click", 2);
	m_Interface->comboBoxClickButton->addItem("Right Click", 8);
	m_Interface->comboBoxClickButton->addItem("Scroll click", 32);

	m_Interface->comboBoxClickType->addItem("Single Click", 1);
	m_Interface->comboBoxClickType->addItem("Double Click", 2);

	// Set false before start.
	QMetaObject::invokeMethod(m_Interface->pushButtonStop, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, false));

	g_Settings->Load();
	g_Settings->Apply(m_Interface);

	UpdateStartStopText();
	m_KeyBinder.Bind(g_Settings->m_KeyStart, &AClicker::KeyListener, this);
	m_KeyBinder.Bind(g_Settings->m_KeyStop, &AClicker::KeyListener, this);
}

AClicker::~AClicker() {}

void AClicker::OnStartPressed() {
	TOGGLE_CLICKER(m_Interface);

	int interval = g_Settings->m_Minutes * 1000 * 60 + g_Settings->m_Seconds * 1000 + g_Settings->m_Milliseconds;
	int clickButton = m_Interface->comboBoxClickButton->itemData(g_Settings->m_ClickButton).toInt();

	if(m_Thread == nullptr && g_Settings->m_IsRunning) {
		m_Thread = new std::thread(&AClicker::StartClicker, this, interval, g_Settings->m_ClickCount, clickButton, g_Settings->m_ClickType + 1);
		m_Thread->detach();
	}
}
void AClicker::OnStopPressed() {
	TOGGLE_CLICKER(m_Interface);
}

ASettings *windowSettings;
void AClicker::OnSettingsPressed() {
	if(windowSettings == nullptr && g_Settings->m_IsRunning == false) {
		AClicker::m_Locked = true;

		windowSettings = new ASettings(nullptr, this);
		windowSettings->show();
	}
}
void ASettings::closeEvent(QCloseEvent *) {
	AClicker::m_Locked = false;

	windowSettings->close();
	delete windowSettings;
	windowSettings = nullptr;
}

void AClicker::OnMinutesEdited() {
	g_Settings->m_Minutes = CLAMP_TO_MIN(m_Interface->lineEditMinutes->text().toInt(), 0);
	m_Interface->lineEditMinutes->setText(QString::number(g_Settings->m_Minutes));
}
void AClicker::OnSecondsEdited() {
	g_Settings->m_Seconds = CLAMP_TO_MIN(m_Interface->lineEditSeconds->text().toInt(), 0);
	m_Interface->lineEditSeconds->setText(QString::number(g_Settings->m_Seconds));
}
void AClicker::OnMillisecondsEdited() {
	g_Settings->m_Milliseconds = CLAMP_TO_MIN(m_Interface->lineEditMilliseconds->text().toInt(), 0);
	m_Interface->lineEditMilliseconds->setText(QString::number(g_Settings->m_Milliseconds));
}
void AClicker::OnClickCountChanged() {
	g_Settings->m_ClickCount = CLAMP_TO_MIN(m_Interface->lineEditRepeatTimes->text().toInt(), 0);
	m_Interface->lineEditRepeatTimes->setText(QString::number(g_Settings->m_ClickCount));
}
void AClicker::OnClickCountInfinitePressed() {
	g_Settings->m_ClickCount = -1;
}
void AClicker::OnClickButtonChanged(int index) {
	g_Settings->m_ClickButton = index;
}
void AClicker::OnClickTypeChanged(int index) {
	g_Settings->m_ClickType = index;
}

void AClicker::closeEvent(QCloseEvent *) {
	g_Settings->Save();
}

void AClicker::UpdateStartStopText() {
	m_Interface->pushButtonStart->setText(QString("Start (") + QChar(g_Settings->m_KeyStart) + QString(")"));
	m_Interface->pushButtonStop->setText(QString("Stop (") + QChar(g_Settings->m_KeyStop) + QString(")"));

	m_KeyBinder.Bind(g_Settings->m_KeyStart, &AClicker::KeyListener, this);
	m_KeyBinder.Bind(g_Settings->m_KeyStop, &AClicker::KeyListener, this);
}

void AClicker::KeyListener(int key, bool pressed) {
	if(pressed && m_Locked == false) {
		if(key == g_Settings->m_KeyStart && g_Settings->m_IsRunning == false)
			OnStartPressed();
		else if(key == g_Settings->m_KeyStop && g_Settings->m_IsRunning == true)
			OnStopPressed();
	}
}

void AClicker::Click(int clickButton) {
	INPUT Input = {0};

	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = clickButton;
	SendInput(1, &Input, sizeof(INPUT));

	ZeroMemory(&Input, sizeof(INPUT));

	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = clickButton * 2;
	SendInput(1, &Input, sizeof(INPUT));
}

void AClicker::StartClicker(int interval, int repetitions, int clickOption, int clickTimes) {
	while(g_Settings->m_IsRunning && repetitions != 0) {
		Click(clickOption);

		if(clickTimes == 2) {
			Click(clickOption);
		}

		if(repetitions > 0) repetitions--;
		std::this_thread::sleep_for(std::chrono::milliseconds(interval));
	}

	// Se opreste automat doar daca se finalizeaza.
	// Altfel s-ar apasat de doua ori.
	if(repetitions == 0)
		OnStopPressed();

	m_Thread = nullptr;
}

bool AClicker::m_Locked;

//153