#pragma once

#include <QtWidgets/QMainWindow>
#include <thread>

#include "ui_AClicker.h"
#include "Settings.h"
#include "Binder.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class AClicker; }
QT_END_NAMESPACE

class AClicker: public QMainWindow {
	Q_OBJECT

public:
	AClicker(QWidget *parent = Q_NULLPTR);
	~AClicker() {}

private:
	void ToggleStartStop();
	void KeyListener(uint8_t key, bool pressed);
	void Click(int clickButton);
	void StartClicker(int interval, int repetitions, int clickOption, int clickTimes);

	int ClampToMin(int n, int min);

private slots:
	void OnStartButtonPressed();
	void OnStopButtonPressed();

	void OnMinutesEdited();
	void OnSecondsEdited();
	void OnMillisecondsEdited();

	void OnRepeatTimesChanged();

private:
	Ui::AClicker *ui;
	std::thread *m_clickerThread;
	Binder<AClicker> m_keyBinder;
};

