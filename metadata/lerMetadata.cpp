#include <iostream>
#include <sstream>
#include <vtkDICOMReader.h>
#include <vtkSmartPointer.h>
#include <vtkDICOMMetaData.h>

int main(int argc, char** argv){
	//Carrega o arquivo
	vtkSmartPointer<vtkDICOMReader> reader = vtkSmartPointer<vtkDICOMReader>::New();
	reader->SetFileName("C:\\meus dicoms\\Phantom 1\\EOT-Secondary\\Phantom1_EOT.49.dcm");
	reader->Update();
	//Imprime a metadata.
	vtkDICOMMetaData *meta = reader->GetMetaData();
	auto iter = meta->Begin();
	while (iter != meta->End()){
		std::stringstream ss;
		vtkDICOMDataElement current = *iter;
		ss << current.GetTag() << ", " << current.GetValue();
		std::cout << ss.str() << std::endl;
		++iter;
	}
	return EXIT_SUCCESS;
}