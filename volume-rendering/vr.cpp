#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <vtkDICOMSorter.h>
#include <vtkStringArray.h>
#include <vtkSmartPointer.h>
#include <vtkDICOMReader.h>
#include <vtkDICOMCTRectifier.h>
#include <vtkMatrix4x4.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolume.h>


vtkSmartPointer<vtkStringArray> GetFileList(std::string filepath){
	std::ifstream infile(filepath);
	vtkSmartPointer<vtkStringArray> fileList = vtkSmartPointer<vtkStringArray>::New();
	for (std::string line; std::getline(infile, line);){
		fileList->InsertNextValue(line.c_str());
	}
	return fileList;
}

int main(int argc, char** argv){
	//Carga do arquivo
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
	vtkSmartPointer<vtkMatrix4x4> patientMatrix = rectifier->GetRectifiedMatrix();
	vtkSmartPointer<vtkImageData> loadedImage = rectifier->GetOutput();

	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderer> ren1 =	vtkSmartPointer<vtkRenderer>::New();
	ren1->SetBackground(0.1, 0.4, 0.2);
	renWin->AddRenderer(ren1);
	renWin->SetSize(301, 300);
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);
	renWin->Render(); // make sure we have an OpenGL context.
	vtkSmartPointer<vtkGPUVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
	volumeMapper->SetBlendModeToComposite(); // composite first
	volumeMapper->SetInputData(loadedImage);
	vtkSmartPointer<vtkVolumeProperty> volumeProperty =	vtkSmartPointer<vtkVolumeProperty>::New();
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

	vtkSmartPointer<vtkVolume> volume =	vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	ren1->AddViewProp(volume);
	ren1->ResetCamera();
	// Render composite. In default mode. For coverage.
	renWin->Render();
	iren->Start();
	return EXIT_SUCCESS;
}