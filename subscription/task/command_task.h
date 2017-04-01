#ifndef COMMAND_TASK_H
#define COMMAND_TASK_H

#include <QObject>
#include <memory>
#include <QString>
#include "dependency.h"
#include <map>
#include "source_declaration.h"

#include "task.h"

#include "command.h"
#include "progress_observer.h"

class Subscription;

class CommandTask : public Task, public ProgressObserver
{
	Q_OBJECT
public:
	explicit CommandTask(QString id, QString target,
	                     std::map<QString, SourceDeclaration> sources)
	    : Task(id, target, sources)
	{}
	virtual ~CommandTask() {}
	virtual bool run() = 0;

	virtual void abort() { ProgressObserver::abort(); }

	bool update(float report, bool incremental = false);

	void setCommand(shell::Command *cmd);

signals:
	void progressChanged(int percent);
	void failure();
	void exception(std::exception_ptr e, bool critical);

protected:
	shell::Command *cmd;

private:

	// progress cached for incremental updates
	float progress;
	// progress in percent
	int percent;
};

#endif // COMMAND_TASK_H
