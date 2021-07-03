#pragma once

#include <QtWidgets/QMainWindow>
#include <thread>

#include "ui_AClicker.h"
#include "Binder.hpp"
#include "Settings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AClicker; }
QT_END_NAMESPACE

class AClicker: public QMainWindow {
	Q_OBJECT

public:
	AClicker(QWidget *parent = nullptr);
	~AClicker();

private slots:
	void OnStartPressed();
	void OnStopPressed();
	void OnSettingsPressed();
	void OnMinutesEdited();
	void OnSecondsEdited();
	void OnMillisecondsEdited();
	void OnClickCountChanged();
	void OnClickCountInfinitePressed();
	void OnClickButtonChanged(int index);
	void OnClickTypeChanged(int index);

public:
	void closeEvent(QCloseEvent *) override;

public:
	void UpdateStartStopText();
	void KeyListener(int key, bool pressed);
	void Click(int clickButton);
	void StartClicker(int interval, int repetitions, int clickOption, int clickTimes);

private:
	Ui::AClicker *m_Interface;
	Binder<AClicker> m_KeyBinder;
	std::thread *m_Thread;

public:
	static bool m_Locked;

};

