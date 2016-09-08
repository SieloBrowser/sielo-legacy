#include "..\includes\SMainWindow.h"

#include <QMessageBox>

SMainWindow::SMainWindow(QWidget* parent) : 
	QMainWindow(parent)
{
	QPushButton* button{ new QPushButton("Hello World", m_tabs) };

	m_tabs->addTab(button, "Button");

	setCentralWidget(m_tabs);

}


SMainWindow::~SMainWindow()
{
}
