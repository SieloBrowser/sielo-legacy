#include "..\includes\SToolBar.h"
#include "..\includes\Actions.h"
#include "..\includes\SMainWindow.h"

SToolBar::SToolBar(const QString& toolBarName, SMainWindow* parent) :
	QToolBar(toolBarName),
	m_parents(parent),
	m_actions(parent->getActions())
{
	m_spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	addWidget(m_spacer1);

	addAction(m_actions->back);

	addWidget(m_spacer2);
		
}


SToolBar::~SToolBar()
{
}
