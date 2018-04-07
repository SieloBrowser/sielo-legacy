#include <QApplication>
#include <QMessageBox>

#include "includes/Manager.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Manager manager{};

    return 0;
}
