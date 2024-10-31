#include <QCoreApplication>
# include <MLMODELS/mlpproblem.h>
# include <MLMODELS/rbfproblem.h>
# include <GE/grammargenetic.h>
# include <QDebug>

MlpProblem *mlp=NULL;
RbfProblem *rbf=NULL;
GrammarGenetic *gen=NULL;
QVector<Parameter> mainParams;

void makeMainParams()
{
    mainParams<<Parameter("help","","Show help screen");
    QStringList gnnMethod;
    gnnMethod<<"bfgs"<<"gradient"<<"adam"<<"genetic"<<"pso";
    mainParams<<Parameter("gnn_method",gnnMethod[0],gnnMethod,"Used Optimization method");
    mainParams<<Parameter("gnn_model","mlp","Model name. Values: mlp,rbf");
    mainParams<<Parameter("gnn_seed",1,0,100,"Random Seed");
    mainParams<<Parameter("gnn_iters",1,1,100,"Number of iterations");
}

void loadPointers()
{
    mlp = new MlpProblem();
    rbf = new RbfProblem();
}

void unloadPointers()
{
    if(mlp!=NULL)
        delete mlp;
    if(rbf!=NULL)
        delete rbf;
    if(gen!=NULL)
        delete gen;
}

void printOption(QString fullName,
                 QString description,QString value)
{
    QString s=QString("--%1 %2 %3 %4").arg(fullName,-27).
                arg(description,-35).arg("Default value:",-15).arg(value,-10);
    qDebug().noquote()<<s;
}

void checkParameter(
    QString name,QString value,
    QString fullName,
    QString &parameter)
{
    if(name == fullName)
        parameter = value;
}

void error(QString message)
{
    printf("Fatal error %s \n",message.toStdString().c_str());
    qApp->exit(0);
    exit(0);
}

void    printHelp()
{

}

void parseCmdLine(QStringList args)
{
    QString lastParam="";
    for(int i=1;i<args.size();i++)
    {

        if(args[i]=="--help") printHelp();
        int i1=args[i].indexOf("=");
        QString name = args[i].mid(0,i1);
        QString value = args[i].mid(i1+1);
        if(name.startsWith("--"))
            name = name.mid(2);
        if(value=="")
        {
            error(QString("Param %1 is empty.").arg(value));
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);
    setlocale(LC_ALL,"C");
    makeMainParams();
    parseCmdLine(app.arguments());
    return 0;
}
