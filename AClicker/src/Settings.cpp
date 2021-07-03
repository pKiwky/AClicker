#include <string>
#include <windows.h>
#include <AClicker.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Settings.h"

using boost::property_tree::ptree;

Settings *g_Settings = nullptr;

Settings::Settings():
	m_KeyStart(90), m_KeyStop(90),
	m_Minutes(0), m_Seconds(0), m_Milliseconds(100),
	m_ClickCount(-1), m_ClickButton(0), m_ClickType(0),
	m_IsRunning(false) {
}

void Settings::Save() {
	ptree sTree;

	sTree.put("key_start",		/**/ m_KeyStart);
	sTree.put("key_stop",		/**/ m_KeyStop);
	sTree.put("minutes",		/**/ m_Minutes);
	sTree.put("seconds",		/**/ m_Seconds);
	sTree.put("milliseconds",	/**/ m_Milliseconds);
	sTree.put("click_count",	/**/ m_ClickCount);
	sTree.put("click_button",	/**/ m_ClickButton);
	sTree.put("click_type",		/**/ m_ClickType);

	write_json("ASettings.json", sTree);
}

void Settings::Load() {
	ptree sTree;

	try {
		read_json("ASettings.json", sTree);

		m_KeyStart		/**/ = sTree.get<int>("key_start", 90);
		m_KeyStop		/**/ = sTree.get<int>("key_stop", 90);
		m_Minutes		/**/ = sTree.get<int>("minutes", 0);
		m_Seconds		/**/ = sTree.get<int>("seconds", 0);
		m_Milliseconds	/**/ = sTree.get<int>("milliseconds", 100);
		m_ClickCount	/**/ = sTree.get<int>("click_count", 0);
		m_ClickButton	/**/ = sTree.get<int>("click_button", 0);
		m_ClickType		/**/ = sTree.get<int>("click_type", 0);
	} catch(boost::property_tree::ptree_bad_path const &badPathException) {
		qDebug() << "Exception caught for bad path: " << badPathException.what();
	} catch(boost::property_tree::ptree_bad_data const &badDataException) {
		qDebug() << "Exception caught for bad data: " << badDataException.what();
	} catch(std::exception const &exception) {
		qDebug() << "Exception caught when parsing message into Boost.Property tree: " << exception.what();
	}
}

void Settings::Apply(Ui::AClicker *ui) {
	ui->lineEditMinutes->setText(QString::number(m_Minutes));
	ui->lineEditSeconds->setText(QString::number(m_Seconds));
	ui->lineEditMilliseconds->setText(QString::number(m_Milliseconds));
	ui->comboBoxClickButton->setCurrentIndex(m_ClickButton);
	ui->comboBoxClickType->setCurrentIndex(m_ClickType);

	if(m_ClickCount != -1) {
		ui->lineEditRepeatTimes->setText(QString::number(m_ClickCount));
		ui->radioButtonRepeatTimes->setChecked(true);
	}
	else {
		ui->radioButtonRepeatInf->setChecked(true);
	}
}
