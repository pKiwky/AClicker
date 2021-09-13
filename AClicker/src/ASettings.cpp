#include "Settings.h"
#include "ASettings.h"

#include <Windows.h>
#include <conio.h>
#include <QKeyEvent>
#include <iostream>

enum class ListenerType {
	None = 0,
	Start = 1,
	Stop = 2
};

extern Settings *g_Settings;

ASettings::ASettings(QWidget *parent, AClicker *mainWindow): QWidget(parent), m_Interface(new Ui::ASettings), m_MainWindow(mainWindow) {
	m_Interface->setupUi(this);

	setWindowTitle("Settings");
	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

	connect(m_Interface->pushButtonStart, SIGNAL(pressed()), this, SLOT(OnStartBindPressed()));
	connect(m_Interface->pushButtonStop, SIGNAL(pressed()), this, SLOT(OnStopBindPressed()));

	installEventFilter(this);

	m_Interface->lineEditStart->setText(QChar(g_Settings->m_KeyStart));
	m_Interface->lineEditStop->setText(QChar(g_Settings->m_KeyStop));
}

ASettings::~ASettings() {}

bool ASettings::eventFilter(QObject *obj, QEvent *event) {
	QKeyEvent *key = static_cast<QKeyEvent *>(event);

	if(event->type() == QEvent::KeyPress) {
		QKeyEvent *key = static_cast<QKeyEvent *>(event);
		// Bug problem with special keys.
		// Temporary fix.
		if(key->key() > 255)
			return false;

		switch(m_KeyListenerType) {
			case (int)ListenerType::Start:
			{
				g_Settings->m_KeyStart = key->key();
				m_Interface->lineEditStart->setText(QChar(key->key()));
				break;
			}
			case (int)ListenerType::Stop:
			{
				g_Settings->m_KeyStop = key->key();
				m_Interface->lineEditStop->setText(QChar(key->key()));
				break;
			}
			default:
				break;
		}
		m_KeyListenerType = (int)ListenerType::None;
		m_MainWindow->UpdateStartStopText();
		return true;
	}
	else {
		return QObject::eventFilter(obj, event);
	}
	return false;
}

void ASettings::OnStartBindPressed() {
	m_KeyListenerType = (int)ListenerType::Start;
	m_Interface->lineEditStart->setText("Press key");
}

void ASettings::OnStopBindPressed() {
	m_KeyListenerType = (int)ListenerType::Stop;
	m_Interface->lineEditStop->setText("Press key");
}
