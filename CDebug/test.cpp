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

	
d.NewSection(true, "Tabulations sample");
	d.InLineSetQuantity(4);
	for (int i = 0; i < 16; ++i) {
		d.InLine(i, "\t");
	}

	d.NewSection(true);

	d.Line("vstavit kortenky ;ja sj;kasd fkj;l dfsajk; as dfasdf asdf asdf asdfasd ");
	d.SetSectionAlignment(Debug::ALIGN_RIGHT);

d.End();
	std::cin.get();


}