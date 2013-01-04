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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Computations :                                                  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   This code shouldn't be changed. It computes hardcoded values  %
%   in order to be used by your C program.                        %
%   This code should be only changed to fix bugs, or mathematical %
%   modelling errors                                              %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

base = gf(2, galois_size, primitive_polynomial);
hex_size = log2(galois_size);
printf('rcon[] = {\n');
for value=1:(2**galois_size)
    if (value ~= 1)
        printf(', ')
        if mod(value-1, galois_size) == 0
            printf('\n')
        end
    end
    rcon = base**value;
    printf('0x%s', dec2hex(rcon.x, hex_size));
end
printf('\n}\n');
