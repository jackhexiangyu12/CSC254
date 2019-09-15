with Ada.Command_Line; use Ada.Command_Line;
with Ada.Text_IO; use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;

procedure Trees is
    procedure Gen is
        N : Integer := Integer'Value (Argument (1));
    begin
        Put (N);
    end Gen;
begin
    Gen;
end Trees;