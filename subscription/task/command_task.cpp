#include "command_task.h"

#include <QDebug>
#include <cassert>
#include "subscription.h"

void CommandTask::setCommand(shell::Command *cmd)
{
	cmd->setProgressObserver(this);
	this->cmd = cmd;
}

bool CommandTask::update(float report, bool incremental)
{
	//GGDBGM("CommandRunner object " << this << endl);
	progress = (incremental ? progress + report : report);

	// propagate if percentage changed, don't heat up GUI unnecessarily
	if ((progress * 100) > percent) {
		percent = progress * 100;
		emit progressChanged(percent);
	}
	return !isAborted();
}

void CommandTask::cancel()
{
	disconnect(this, SIGNAL(progressChanged(int)), 0, 0);
	ProgressObserver::abort();
}
