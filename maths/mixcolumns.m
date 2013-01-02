% vim: set fileencoding=utf-8 expandtab tabstop=4

% Copyright (c) 2012, Antoine Catton and Jessy Mauclair-Richalet
% 
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to
% deal in the Software without restriction, including without limitation the
% rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
% sell copies of the Software, and to permit persons to whom the Software is
% furnished to do so, subject to the following conditions:
% 
% The above copyright notice and this permission notice shall be included in
% all copies or substantial portions of the Software.
% 
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
% FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
% IN THE SOFTWARE.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Values :                                                        %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   This values can be changed in order to change the size of the %
%   algorithm, or change constants.                               %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

galois_size = 16;

% This primitive polynomial correspond to D^16+D^13+D^11+D^7+D^5+D^3+1.
% It was chosen because there's lot of prime powers, and because
% prime numbers are good in cryptography.
% DISCLAIMER : In this context prime numbers doesn't guarantee
%              any improved security or whatsoever.
primitive_polynomial = 2**16 + 2**13 + 2**11 + 2**7 + 2**5 + 2**3 + 1;
% Primitive polynomial list can be found with :
%   octave> primpoly(galois_field_size, 'all')

% X_matrix is a circulant matrix of the 5 biggest prime
% number lesser than 2^16 = 65536
X_matrix = toeplitz([65479 65497 65519 65520],
                    [65479 65520 65519 65497]);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Computations :                                                  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   This code shouldn't be changed. It computes hardcoded values  %
%   in order to be used by your C program.                        %
%   This code should be only changed to fix bugs, or mathematical %
%   modelling errors                                              %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function name = lookup_table_name(number)
    name = sprintf('lookup_%s_table', dec2hex(number));
end

galois_X_matrix = gf(X_matrix, galois_size, primitive_polynomial);
inv_galois_X_matrix = inv(galois_X_matrix);
inv_X_matrix = inv_galois_X_matrix.x;

% Get each uniq element
uniq_elements = unique([galois_X_matrix.x, inv_galois_X_matrix.x])';

% Create tables for each unique element lookup table
for element=uniq_elements
    galois_element = gf(element, galois_size, primitive_polynomial);
    printf('%s[] = {\n', lookup_table_name(element));
    for value=0:((2**galois_size)-1)
        if (value ~= 0)
            printf(', ');
            if (mod(value, galois_size) == 0)
                printf('\n');
            end
        end
        galois_value = gf(value, galois_size, primitive_polynomial);
        galois_result = galois_value * galois_element;
        result = galois_result.x;
        printf('0x%s', dec2hex(result, log2(galois_size)));
    end
    printf('\n};\n')
end

% Create associative array
printf('lookup_table_list[] = {\n');
for element=uniq_elements
    printf('{0x%s, &%s},\n', dec2hex(element, log2(galois_size)), lookup_table_name(element));
end
printf('NULL\n};\n');

function print_matrix(name, matrix, galois_size)
    x_size = size(matrix)(2);
    printf('%s[MIXCOLUMNS_SIZE_Y][MIXCOLUMNS_SIZE_X] = {\n', name);
    vector = reshape(matrix', 1, []);
    vector_size = size(vector)(2);
    for i=1:vector_size
        c_index = i-1;
        if (c_index ~= 0)
            printf(', ')
            if (mod(c_index, x_size) == 0)
                printf('\n');
            end
        end
        printf('0x%s', dec2hex(vector(i), log2(galois_size)));
    end
    printf('\n};\n');
end

[y_size, x_size] = size(X_matrix);

printf('#define MIXCOLUMNS_SIZE_X %d\n', x_size);
printf('#define MIXCOLUMNS_SIZE_Y %d\n', y_size);

print_matrix('mixcolumn_matrix', X_matrix, galois_size);
print_matrix('invmixcolumn_matrix', inv_X_matrix, galois_size);
