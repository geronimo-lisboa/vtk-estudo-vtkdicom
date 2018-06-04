unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs;

type
  TForm1 = class(TForm)
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure FormPaint(Sender: TObject);
  private
    //Coisas relativas à dll.
    handleDll:Cardinal;
   DllCreateScreen:procedure(h:HWND);stdcall;
    DllMouseMove:function(h:HWND; nFlags:LongWord; x:integer; y:integer):integer;stdcall;
    DllLeftMouseDown:function(h:HWND; nFlags:LongWord; x:integer; y:integer):integer;stdcall;
    DllLeftMouseUp:function(h:HWND; nFlags:LongWord; x:integer; y:integer):integer;stdcall;
    DllMiddleMouseDown:function(h:HWND; nFlags:LongWord; x:integer; y:integer):integer;stdcall;
    DllMiddleMouseUp:function(h:HWND; nFlags:LongWord; x:integer; y:integer):integer;stdcall;
    DllRightMouseDown:function(h:HWND; nFlags:LongWord; x:integer; y:integer):integer;stdcall;
    DllRightMouseUp:function(h:HWND; nFlags:LongWord; x:integer; y:integer):integer;stdcall;
    DllResize:procedure(w,h:integer);stdcall;
    DllRender:procedure();stdcall;
  public
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.FormCreate(Sender: TObject);
begin
  handleDll := LoadLibrary('F:\teste-vtk-dicom-lib\build\Debug\volume_rendering.dll');
  DllCreateScreen := GetProcAddress(handleDll, '_CreateScreen@4');
  DllMouseMove := GetProcAddress(handleDll, '_LeftMouseDown@16');
  DllLeftMouseDown := GetProcAddress(handleDll, '_LeftMouseDown@16');
  DllLeftMouseUp := GetProcAddress(handleDll, '_LeftMouseUp@16');
  DllMiddleMouseDown := GetProcAddress(handleDll, '_MiddleMouseDown@16');
  DllMiddleMouseUp := GetProcAddress(handleDll, '_MiddleMouseUp@16');
  DllRightMouseDown := GetProcAddress(handleDll, '_RightMouseDown@16');
  DllRightMouseUp := GetProcAddress(handleDll, '_RightMouseUp@16');
  DllResize := GetProcAddress(handleDll, '_Resize@8');
  DllRender := GetProcAddress(handleDll, '_Render@0');
end;

procedure TForm1.FormShow(Sender: TObject);
begin
  DllCreateScreen(Self.Handle);
  DllResize(Self.width, Self.Height);
end;

procedure TForm1.FormPaint(Sender: TObject);
begin
  DllRender();
end;

end.
 