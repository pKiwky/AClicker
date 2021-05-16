#pragma once

#include <thread>
#include <mutex>
#include <map>
#include <iostream>
#include <Windows.h>

template <typename T>
using KeyCallBack = void(T:: *)(uint8_t, bool);

template <typename T>
struct FunctionBinder {
	KeyCallBack<T> callBack;
	T *className;
	bool isKeyDown;
};

template <typename T>
class Binder {

public:
	Binder();
	~Binder();

	void Bind(uint8_t key, KeyCallBack<T> callBack, T *className);
	void Unbind(uint8_t key);
	void KeyListener(std::map<uint8_t, FunctionBinder<T>> *keys);

private:
	std::thread *m_KeyThread;
	std::map<uint8_t, FunctionBinder<T>> *m_KeyList;
	bool isRunning;
	std::mutex m_Mutex;

};


template<typename T>
Binder<T>::Binder() {
	m_KeyList = new std::map<uint8_t, FunctionBinder<T>>();

	isRunning = true;
	m_KeyThread = new std::thread(&Binder::KeyListener, this, m_KeyList);
	m_KeyThread->detach();
}

template<typename T>
Binder<T>::~Binder() {
	isRunning = false;
	std::lock_guard<std::mutex> lock(m_Mutex);
}

template<typename T>
void Binder<T>::Bind(uint8_t key, KeyCallBack<T> callBack, T *className) {
	FunctionBinder<T> binderFunction;
	binderFunction.callBack = callBack;
	binderFunction.className = className;
	binderFunction.isKeyDown = false;

	m_KeyList->insert({key, binderFunction});
}

template <typename T>
void Binder<T>::Unbind(uint8_t key) {
	std::lock_guard<std::mutex> lock(m_Mutex);
	{
		m_KeyList->erase(key);
	}
}

template<typename T>
void Binder<T>::KeyListener(std::map<uint8_t, FunctionBinder<T>> *keys) {
	uint8_t anyKeyDown = 0;

	while(isRunning) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		{
			for(auto it = keys->begin(); it != keys->end(); it++) {
				uint8_t key = it->first;

				if(GetKeyState(key) < 0) {
					anyKeyDown = key;

					FunctionBinder<T> binderFunction = it->second;
					(binderFunction.className->*binderFunction.callBack)(key, true);
					it->second.isKeyDown = true;

					break;
				}
				if(GetKeyState(key) >= 0 && it->second.isKeyDown) {
					FunctionBinder<T> binderFunction = it->second;
					(binderFunction.className->*binderFunction.callBack)(key, false);
					it->second.isKeyDown = false;

					break;
				}
			}
		}
		while(GetKeyState(anyKeyDown) < 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	delete keys;
}
