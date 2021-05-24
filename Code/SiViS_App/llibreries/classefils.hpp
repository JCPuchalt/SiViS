#pragma once

#include "llibreries.hpp"

/*
*
* @ 
*
*/ 
class classefils
{
	
public:
	sistema_visio* SV;

	classefils() {}
	virtual ~classefils() {}

	// Returns true if the thread was successfully started, false if there was an error starting the thread 
	bool StartInternalThread( sistema_visio* SisVis )
	{
		SV = SisVis;
		return (pthread_create(&_thread, NULL, InternalThreadEntryFunc, this) == 0);
	}

	// Will not return until the internal thread has exited. 
	void WaitForInternalThreadToExit()
	{
		(void) pthread_join(_thread, NULL);
	}

protected:
	// Implement this method in your subclass with the code you want your thread to run. 
	virtual void InternalThreadEntry() = 0;

private:
	static void * InternalThreadEntryFunc(void * This) {((classefils *)This)->InternalThreadEntry(); return NULL;}

	pthread_t _thread;

};



/*
* @ Classe herencia
*/
class fils : public classefils
{

public:
	fils() {}
	~fils() {}

	void InternalThreadEntry ();

};
