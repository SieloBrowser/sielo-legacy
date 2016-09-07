#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget* fen{ new QWidget };
    fen->show();

    return app.exec();
}
