#include "Debug.h"
#include <string>
#include <iostream>

int main() { 


Debug d("main");
d.SetSubject("Logger");
	d.Line("Example line");
	
d.NewSection(true, "Inline example with quantity option");
	
	d.InLineSetQuantity(2);
	
	for (int i = 0; i < 10; ++i) {
		d.InLine(i, " ");
	}

d.NewSection(true, "Alignment sample");
	d.SetSectionAlignment(Debug::ALIGN_RIGHT);
	d.Line("Some text");
	
d.NewSection(true, "Tabulations sample");
	d.InLineSetQuantity(4);
	for (int i = 0; i < 16; ++i) {
		d.InLine(i, "\t");
	}

d.NewSection(true, "Too long string sample");
	d.InLineResetQuantity();
	d.DisableAutoSpacing();
	for (int i = 0; i < 100; ++i) {
		d.InLine(i, " ");
	}
	

d.End();
	std::cin.get();



}