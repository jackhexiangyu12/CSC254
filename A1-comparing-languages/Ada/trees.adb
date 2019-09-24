with Ada.Command_Line; use Ada.Command_Line;
with Ada.Text_IO; use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;
with Ada.Containers; use Ada.Containers;
with Ada.Containers.Vectors;
with Ada.Strings; use Ada.Strings;
with Ada.Strings.Unbounded; use Ada.Strings.Unbounded;

procedure Trees is
    procedure Gen is
        package String1_Vectors is 
            new Ada.Containers.Vectors (Index_Type => Natural, Element_Type => Unbounded_String);
        package String2_Vectors is 
            new Ada.Containers.Vectors (Index_Type => Natural, Element_Type => String1_Vectors.Vector, "=" => String1_Vectors."=");

        N : Integer;
        R : String2_Vectors.Vector;
    begin
        N := Integer'Value (Argument (1));

        -- When N is 0
        declare
            T : String1_Vectors.Vector;
        begin
            T.Append (To_Unbounded_String (""));
            R.Append (T);
        end;

        if N > 0 then
            -- When N is 1
            declare
                T : String1_Vectors.Vector;
            begin
                T.Append (To_Unbounded_String ("(.)"));
                R.Append (T);
            end;

            -- When N is greater than 1
            for I in 2 .. N loop
                declare
                    T : String1_Vectors.Vector;
                begin
                    -- For all possible leftsubtree with J nodes and rightsubtree with I-J-1 nodes
                    for J in 0 .. I-1 loop
                        for Left of R (J) loop
                            for Right of R (I-J-1) loop
                                -- Add leftsubtree and rightsubtree to root and store in list
                                T.Append (To_Unbounded_String("(" & To_String (Left) & "." & To_String (Right) & ")"));
                            end loop;
                        end loop;
                    end loop;
                    R.Append (T);
                end;
            end loop;
        end if;

        for Tree of R.Last_Element loop
            Put_Line (To_String (Tree));
        end loop;
        
    end Gen;
begin
    Gen;
end Trees;