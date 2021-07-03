#pragma once

#include <QWidget>
#include "ui_ASettings.h"

class AClicker;

class ASettings: public QWidget {
	Q_OBJECT

public:
	ASettings(QWidget *parent = nullptr, AClicker *mainWindow = nullptr);
	~ASettings();

public:
	void closeEvent(QCloseEvent *) override;
	bool eventFilter(QObject *obj, QEvent *event);

public slots:
	void OnStartBindPressed();
	void OnStopBindPressed();

private:
	AClicker *m_MainWindow;
	Ui::ASettings *m_Interface;
	int m_KeyListenerType;

};
