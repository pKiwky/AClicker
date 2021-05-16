#include "AClicker.h"

#define ACLICKER_VERSION "1.0.0"

#if _DEBUG
#define ACLICKER_BUILD "Debug"
#else
#define ACLICKER_BUILD "Release"
#endif

extern Settings *g_Settings;

AClicker::AClicker(QWidget *parent): QMainWindow(parent), ui(new Ui::AClicker), m_clickerThread(nullptr) {
	ui->setupUi(this);
	g_Settings = new Settings();

	setWindowTitle("AClicker " + QString(ACLICKER_VERSION) + " " + QString(ACLICKER_BUILD));
	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

	ui->pushButtonStart->setText(QString::asprintf("Start (Press %c)", g_Settings->m_keyToggle));
	ui->pushButtonStop->setText(QString::asprintf("Stop (Press %c)", g_Settings->m_keyToggle));
	ui->pushButtonStop->setEnabled(false);

	connect(ui->pushButtonStart, SIGNAL(pressed()), this, SLOT(OnStartButtonPressed()));
	connect(ui->pushButtonStop, SIGNAL(pressed()), this, SLOT(OnStopButtonPressed()));

	connect(ui->lineEditMinutes, SIGNAL(editingFinished()), this, SLOT(OnMinutesEdited()));
	connect(ui->lineEditSeconds, SIGNAL(editingFinished()), this, SLOT(OnSecondsEdited()));
	connect(ui->lineEditMilliseconds, SIGNAL(editingFinished()), this, SLOT(OnMillisecondsEdited()));

	connect(ui->lineEditRepeatTimes, SIGNAL(editingFinished()), this, SLOT(OnRepeatTimesChanged()));

	m_keyBinder.Bind(g_Settings->m_keyToggle, &AClicker::KeyListener, this);

	ui->comboBoxClickButton->addItem("Left Click", 2);
	ui->comboBoxClickButton->addItem("Right Click", 8);
	ui->comboBoxClickButton->addItem("Scroll click", 32);

	ui->comboBoxClick->addItem("Single Click", 1);
	ui->comboBoxClick->addItem("Double Click", 2);
}

void AClicker::ToggleStartStop() {
	g_Settings->m_isRunning = !g_Settings->m_isRunning;

	QMetaObject::invokeMethod(ui->pushButtonStart, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, !g_Settings->m_isRunning));
	QMetaObject::invokeMethod(ui->pushButtonStop, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, g_Settings->m_isRunning));
}

void AClicker::KeyListener(uint8_t key, bool pressed) {
	if(pressed) {
		g_Settings->m_isRunning ? OnStopButtonPressed() : OnStartButtonPressed();
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
	while(g_Settings->m_isRunning && repetitions != 0) {
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
		OnStopButtonPressed();

	m_clickerThread = nullptr;
}

int AClicker::ClampToMin(int n, int min) {
	return n < min ? min : n;
}

void AClicker::OnStartButtonPressed() {
	int minutes = ui->lineEditMinutes->text().toInt();
	int seconds = ui->lineEditSeconds->text().toInt();
	int ms = ui->lineEditMilliseconds->text().toInt();

	int interval = minutes * 1000 * 60 + seconds * 1000 + ms;
	int repetitions = ui->radioButtonRepeatInf->isChecked() ? -1 : ui->lineEditRepeatTimes->text().toInt();

	int clicksIndex = ui->comboBoxClick->currentIndex();
	int clicks = ui->comboBoxClick->itemData(clicksIndex).toInt();

	int clickIndex = ui->comboBoxClickButton->currentIndex();
	int clickButton = ui->comboBoxClickButton->itemData(clickIndex).toInt();

	if(interval == 0) return;
	if(ui->radioButtonRepeatInf->isChecked() == false && ui->radioButtonRepeatTimes->isChecked() == false)
		return;

	ToggleStartStop();
	if(m_clickerThread == nullptr && g_Settings->m_isRunning) {
		m_clickerThread = new std::thread(&AClicker::StartClicker, this, interval, repetitions, clickButton, clicks);
		m_clickerThread->detach();
	}
}

void AClicker::OnStopButtonPressed() {
	ToggleStartStop();
}

void AClicker::OnMinutesEdited() {
	int minutes = ClampToMin(ui->lineEditMinutes->text().toInt(), 0);
	ui->lineEditMinutes->setText(QString::number(minutes));
}

void AClicker::OnSecondsEdited() {
	int seconds = ClampToMin(ui->lineEditSeconds->text().toInt(), 0);
	ui->lineEditSeconds->setText(QString::number(seconds));
}

void AClicker::OnMillisecondsEdited() {
	int milliseconds = ClampToMin(ui->lineEditMilliseconds->text().toInt(), 0);
	ui->lineEditMilliseconds->setText(QString::number(milliseconds));
}

void AClicker::OnRepeatTimesChanged() {
	int repetitions = ClampToMin(ui->lineEditRepeatTimes->text().toInt(), 0);
	ui->lineEditRepeatTimes->setText(QString::number(repetitions));
}

