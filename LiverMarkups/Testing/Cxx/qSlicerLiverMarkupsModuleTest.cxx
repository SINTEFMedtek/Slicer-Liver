// Qt includes
#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <vtkSlicerApplicationLogic.h>

// Slicer includes
#include "qSlicerLiverMarkupsModule.h"
#include <qSlicerApplication.h>
#include <qSlicerModuleFactoryManager.h>
#include <qSlicerModuleManager.h>
#include <qSlicerApplicationHelper.h>

//Module includes
#include <vtkSlicerLiverMarkupsLogic.h>

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include "qMRMLWidget.h"
#include <vtkTestingOutputWindow.h>

class markupsModuleTest: public qSlicerLiverMarkupsModule
{
public:
    qSlicerApplication app;
    markupsModuleTest(int argc, char * argv[]) :
        app(argc, argv)
    {}

    void startApp()
    {
        //Using code example from qSlicerModelsModuleWidgetTest1

        qMRMLWidget::preInitializeApplication();
        qMRMLWidget::postInitializeApplication();
    }
    int runApp(int argc, char * argv[])
    {
        if (argc < 2 || QString(argv[1]) != "-I")
        {
            QTimer::singleShot(100, &app, SLOT(quit()));
        }
        return app.exec();
    }
	void setup()
    {
        qSlicerLiverMarkupsModule::setup();
	}

    void loadModules()
    {
        // copied from generated qSlicerLiverMarkupsModuleGenericTest
        if (!dependencies().isEmpty())
          {
          qSlicerModuleFactoryManager * moduleFactoryManager = app.moduleManager()->factoryManager();
          qSlicerApplicationHelper::setupModuleFactoryManager(moduleFactoryManager);
          moduleFactoryManager->setExplicitModules(dependencies());

          moduleFactoryManager->registerModules();
          qDebug() << "Number of registered modules:"
                   << moduleFactoryManager->registeredModuleNames().count();

          moduleFactoryManager->instantiateModules();
          qDebug() << "Number of instantiated modules:"
                   << moduleFactoryManager->instantiatedModuleNames().count();

          // Load all available modules
          foreach(const QString& name, moduleFactoryManager->instantiatedModuleNames())
            {
            Q_ASSERT(!name.isNull());
            moduleFactoryManager->loadModule(name);
            }
          }
    }

    int checkMarkupNodes()
    {
        vtkSlicerApplicationLogic* appLogic = app.applicationLogic();
        vtkSlicerMarkupsLogic* markupsLogic = vtkSlicerMarkupsLogic::SafeDownCast(appLogic->GetModuleLogic("Markups"));
        if(!markupsLogic->IsMarkupsNodeRegistered("vtkMRMLMarkupsSlicingContourNode"))
        {
            qCritical() << Q_FUNC_INFO<< "vtkMRMLMarkupsSlicingContourNode isn't registered";
            return 1;
        }
        if(!markupsLogic->IsMarkupsNodeRegistered("vtkMRMLMarkupsDistanceContourNode"))
        {
            qCritical() << Q_FUNC_INFO<< "vtkMRMLMarkupsDistanceContourNode isn't registered";
            return 1;
        }
        if(!markupsLogic->IsMarkupsNodeRegistered("vtkMRMLMarkupsBezierSurfaceNode"))
        {
            qCritical() << Q_FUNC_INFO<< "vtkMRMLMarkupsBezierSurfaceNode isn't registered";
            return 1;
        }
        return 0;
    }
};

//-----------------------------------------------------------------------------
int qSlicerLiverMarkupsModuleTest(int argc, char * argv[] )
{
    markupsModuleTest markupsModule = markupsModuleTest(argc, argv);
    markupsModule.startApp();

	if(!markupsModule.isHidden())
		return 1;

    vtkSmartPointer<vtkMRMLScene> scene = vtkSmartPointer<vtkMRMLScene>::New();
    vtkSlicerApplicationLogic* appLogic = markupsModule.app.applicationLogic();

    markupsModule.loadModules();

    qDebug() << "initialize start";
//    TESTING_OUTPUT_ASSERT_WARNINGS_BEGIN();
    // Set path just to avoid a runtime warning at module initialization
    markupsModule.setPath(markupsModule.app.slicerHome() + '/' + markupsModule.app.slicerSharePath() + "/qt-loadable-modules/LiverMarkups");
    markupsModule.initialize(appLogic);
//    markupsModule.initialize(nullptr);
//    TESTING_OUTPUT_ASSERT_WARNINGS_END(); // warning due to using 0 as application logic

    vtkSlicerLiverMarkupsLogic* logic = vtkSlicerLiverMarkupsLogic::SafeDownCast(markupsModule.logic());
    if(!logic) {
        qCritical() << "No logic";
        return 1;
    }
    qDebug() << "initialize finished";

    qDebug() <<"setMRMLScene start";
    markupsModule.setMRMLScene(scene.GetPointer());
    qDebug() << "setMRMLScene finish";

    int retval = markupsModule.checkMarkupNodes();
    if(retval != 0)
        return retval;

    return markupsModule.runApp(argc, argv);

//	return 0;
}
