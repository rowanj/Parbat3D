#include "PchApp.h"

#include "ViewportListener.h"
#include "console.h"

void ViewportListener::notify_viewport(void)
{
	Console::write("Default notify viewport (probably not what you want)\n");
}

void ViewportListener::notify_bands(void)
{
	Console::write("Default notify bands (probably not what you want)\n");
}
