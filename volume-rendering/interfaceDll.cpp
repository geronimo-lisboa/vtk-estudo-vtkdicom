#include "interfaceDll.h"
#include <iostream>
#include <sstream>
#include <vtkCamera.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkDICOMSorter.h>
#include <vtkDICOMReader.h>
#include <vtkDICOMCTRectifier.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkWin32RenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVolume.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkSeedRepresentation.h>
#include <vtkPointWidget.h>
#include <vtkCommand.h>
#define PrintPosition {	std::stringstream ss;ss << GetCurrentTime() << ", " << __FUNCTION__ << ", " << __LINE__;std::cout << ss.str() << std::endl; }

class EnableEventObserver :public vtkCommand{
public:
	static EnableEventObserver* New(){ return new EnableEventObserver(); }
	void Execute(vtkObject *caller, unsigned long eventId, void *callData){
		PrintPosition;
	}
};

class StartInteractionEventObserver :public vtkCommand{
public:
	static StartInteractionEventObserver* New(){ return new StartInteractionEventObserver(); }
	void Execute(vtkObject *caller, unsigned long eventId, void *callData){
		PrintPosition;
	}
};

class InteractionEventObserver :public vtkCommand{
private:
	vtkSmartPointer<vtkPolyData>PolyData;
	vtkSmartPointer<vtkMatrix4x4> patientMatrix;
	InteractionEventObserver(){
		PolyData = vtkSmartPointer<vtkPolyData>::New();
		patientMatrix = nullptr;
	}
public:
	static InteractionEventObserver* New(){ return new InteractionEventObserver(); }
	void SetPatientMatrix(vtkSmartPointer<vtkMatrix4x4> mat){
		patientMatrix = mat;
	}
	void Execute(vtkObject *caller, unsigned long eventId, void *callData){
		PrintPosition;
		vtkPointWidget *pointWidget = reinterpret_cast<vtkPointWidget*>(caller);
		pointWidget->GetPolyData(this->PolyData);
		double position[3];
		pointWidget->GetPosition(position);
		std::cout << "   cursor: "
			<< std::fixed << std::setprecision(4)
			<< position[0] << ", " << position[1] << ", " << position[2] << std::endl;
	}
};


vtkSmartPointer<vtkStringArray> GetFileList(std::string filepath);


vtkSmartPointer<vtkImageData> loadedImage = nullptr;
vtkSmartPointer<vtkMatrix4x4> patientMatrix = nullptr;
vtkSmartPointer<vtkWin32OpenGLRenderWindow> renWin = nullptr;
vtkSmartPointer<vtkRenderer> ren1 = nullptr;
vtkSmartPointer<vtkWin32RenderWindowInteractor> iren = nullptr;
vtkSmartPointer<vtkPointWidget> pointWidget = nullptr;


void __stdcall CreateScreen(HWND handle){
	PrintPosition;
	/////CARGA DO ARQUIVO
	//Montagem da lista
	vtkSmartPointer<vtkStringArray> fileList = GetFileList("C:\\meus dicoms\\ab aneurisma.txt");
	//ordena a lista
	vtkSmartPointer<vtkDICOMSorter> sorter = vtkSmartPointer<vtkDICOMSorter>::New();
	sorter->SetInputFileNames(fileList);
	sorter->Update();
	vtkSmartPointer<vtkStringArray> sortedFileList = sorter->GetOutputFileNames();
	//A carga do dicom.
	vtkSmartPointer<vtkDICOMReader> reader = vtkSmartPointer<vtkDICOMReader>::New();
	reader->SetFileNames(sortedFileList);
	reader->SetMemoryRowOrderToFileNative();//Impede o flip na carga
	reader->AutoRescaleOff();//Não aplica reslope intercept/rescale
	//O rectifer é para lidar com gantry tilt
	vtkSmartPointer<vtkDICOMCTRectifier> rectifier = vtkSmartPointer<vtkDICOMCTRectifier>::New();
	rectifier->SetVolumeMatrix(reader->GetPatientMatrix());
	rectifier->SetInputConnection(reader->GetOutputPort());
	rectifier->Update();
	//Essa matriz faz a conversão de coordenadas de datos para coordenadas espaciais no sistema de
	//coordenadas do paciente;
	patientMatrix = rectifier->GetRectifiedMatrix();
	loadedImage = rectifier->GetOutput();
	/////CRIACAO DA PIPELINE
	vtkSmartPointer<vtkGPUVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
	volumeMapper->SetBlendModeToComposite(); // composite first
	volumeMapper->SetInputData(loadedImage);
	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->ShadeOff();
	volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

	vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	compositeOpacity->AddPoint(0.0, 0.0);
	compositeOpacity->AddPoint(120.0, 0.001);
	compositeOpacity->AddPoint(200.1, 0.75);
	compositeOpacity->AddPoint(400.0, 0.1);
	volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.

	vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();
	color->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	color->AddRGBPoint(120.0, 1.0, 0.0, 0.0);
	color->AddRGBPoint(200.1, 1.0, 1.0, 0.0);
	color->AddRGBPoint(400.0, 1.0, 1.0, 1.0);
	volumeProperty->SetColor(color);

	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->SetUserMatrix(patientMatrix);

	/////CRIAÇÃO DA TELA
	renWin = vtkSmartPointer<vtkWin32OpenGLRenderWindow>::New();
	//renWin->SetWindowId(handle);
	ren1 = vtkSmartPointer<vtkRenderer>::New();
	ren1->SetBackground(0.1, 0.4, 0.2);
	ren1->GetActiveCamera()->ParallelProjectionOn();
	renWin->AddRenderer(ren1);
	renWin->SetSize(301, 300);
	iren = vtkSmartPointer<vtkWin32RenderWindowInteractor>::New();
	iren->InstallMessageProcOn();
	iren->SetRenderWindow(renWin);
	renWin->Render(); // make sure we have an OpenGL context.
	ren1->AddViewProp(volume);
	ren1->ResetCamera();

	pointWidget = vtkSmartPointer<vtkPointWidget>::New();
	pointWidget->SetProp3D(volume);
	//pointWidget->SetInputData(loadedImage);
	pointWidget->SetInteractor(iren);
	vtkSmartPointer<EnableEventObserver> enable = vtkSmartPointer<EnableEventObserver>::New();
	pointWidget->AddObserver("EnableEvent", enable);
	vtkSmartPointer<StartInteractionEventObserver> start = vtkSmartPointer<StartInteractionEventObserver>::New();
	pointWidget->AddObserver("StartInteractionEvent", start);
	vtkSmartPointer<InteractionEventObserver> interaction = vtkSmartPointer<InteractionEventObserver>::New();
	interaction->SetPatientMatrix(patientMatrix);
	pointWidget->AddObserver("InteractionEvent", interaction);
	pointWidget->On();
	pointWidget->PlaceWidget();




}
int _stdcall MouseMove(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	iren->OnMouseMove(wnd, 0, X, Y);
	Render();
	return 0;
}
int _stdcall LeftMouseDown(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	iren->OnLButtonDown(wnd, 0, X, Y);
	return 0;
}
int _stdcall LeftMouseUp(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	iren->OnLButtonUp(wnd, 0, X, Y);
	return 0;
}
int _stdcall MiddleMouseDown(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	iren->OnMButtonDown(wnd, 0, X, Y);
	return 0;
}
int _stdcall MiddleMouseUp(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	iren->OnMButtonUp(wnd, 0, X, Y);
	return 0;
}
int _stdcall RightMouseDown(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	iren->OnRButtonDown(wnd, 0, X, Y);
	return 0;
}
int _stdcall RightMouseUp(HWND wnd, UINT nFlags, int X, int Y){
	PrintPosition;
	iren->OnRButtonUp(wnd, 0, X, Y);
	return 0;
}
void _stdcall Resize(int w, int h){
	PrintPosition;
	//renWin->SetSize(w, h);
	Render();
}
void _stdcall Render(){
	PrintPosition;
	renWin->Render();
}

vtkSmartPointer<vtkStringArray> GetFileList(std::string filepath){
	PrintPosition;
	std::ifstream infile(filepath);
	vtkSmartPointer<vtkStringArray> fileList = vtkSmartPointer<vtkStringArray>::New();
	for (std::string line; std::getline(infile, line);){
		fileList->InsertNextValue(line.c_str());
	}
	return fileList;
}
