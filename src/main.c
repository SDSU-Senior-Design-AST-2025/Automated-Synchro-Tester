#include <userint.h>
#include "synchro.h"

int main(void){
	int panel; 
	
	//load user interface file
	if ((panel = LoadPanel(0, "synchro.uir", PANEL)) < 0)
		return -1; 
	
	DisplayPanel(panel);
	RunUserInterface();
	DiscardPanel(panel);
	
	return 0;
	
}
