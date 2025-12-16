#include "session.h"
#include "api.h"
#include "ui.h"

int main() {
    Session session;
    API api(session);
    UI ui(api, session);

    ui.MainMenu();
    return 0;
}
