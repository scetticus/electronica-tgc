#ifndef ALERT_H
#define ALERT_H

void alertShow(char *text, char *title);
void alertHandleEvents(SDL_Event *event);
void alertDraw();

#endif
