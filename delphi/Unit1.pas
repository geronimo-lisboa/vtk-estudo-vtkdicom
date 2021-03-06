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
    procedure FormResize(Sender: TObject);
    procedure FormMouseMove(Sender: TObject; Shift: TShiftState; X,
      Y: Integer);
    procedure FormMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure FormMouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
  private
    //Coisas relativas � dll.
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
  DllMouseMove := GetProcAddress(handleDll, '_MouseMove@16');
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

procedure TForm1.FormResize(Sender: TObject);
begin
  DllResize(Self.width, Self.Height);
end;

procedure TForm1.FormMouseMove(Sender: TObject; Shift: TShiftState; X,
  Y: Integer);
begin
  DllMouseMove(self.handle, 0, X,Y);
end;

procedure TForm1.FormMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  if(Button=mbLeft)then
  begin
    DllLeftMouseDown(self.handle, 0, X,Y);
  end;
  if(Button=mbMiddle)then
  begin
    DllMiddleMouseDown(self.handle, 0, X,Y);
  end;
  if(Button=mbRight)then
  begin
    DllRightMouseDown(self.handle, 0, X,Y);
  end;
end;

procedure TForm1.FormMouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  if(Button=mbLeft)then
  begin
    DllLeftMouseUp(self.handle, 0, X,Y);
  end;
  if(Button=mbMiddle)then
  begin
    DllMiddleMouseUp(self.handle, 0, X,Y);
  end;
  if(Button=mbRight)then
  begin
    DllRightMouseUp(self.handle, 0, X,Y);
  end;
end;

end.
 