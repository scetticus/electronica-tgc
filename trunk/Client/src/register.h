#ifndef REGISTER_H
#define REGISTER_H

void registerDraw();
void registerHandleEvents(SDL_Event *event);
void registerUpdate(const string &field1, const string &field2, const string &field3, const string &field4, const string &button, const string &title);
extern char passwordMd5[33];
extern string regData[4];

#endif
