#include "Debug.h"
#include <string>
#include <iostream>

int main() {
Debug d("main");
	
	d.Line("Example line");
	
d.NewSection(true, "Inline example with quantity option");
	
	d.InLineSetQuantity(2);
	
	for (int i = 0; i < 10; ++i) {
		d.InLine(i, " ");
	}

d.NewSection(true, "Alignment sample");

	d.SetSectionAlignment(Debug::ALIGN_RIGHT);
	d.Line("kyky");

d.NewSection(true, "Tabulations sample");
	d.InLineSetQuantity(4);
	for (int i = 0; i < 16; ++i) {
		d.InLine(i, "\t");
	}
d.End();
	std::cin.get();
}