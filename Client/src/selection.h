#ifndef SELECTION_H
#define SELECTION_H

void selectionHandleEvents(SDL_Event *event);
void selectionDraw();
void selectionUpdate(const string &title, const string &first, const string &second);

#endif
