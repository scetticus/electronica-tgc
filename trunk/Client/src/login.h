#ifndef LOGIN_H
#define LOGIN_H

void loginWindowUpdate(const string &username, int pwdLen);
void loginDraw();
void loginHandleEvents(SDL_Event *event);

#endif
