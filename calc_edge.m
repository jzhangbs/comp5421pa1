function adj = calc_edge(img)
    f = zeros(3, 3, 8);
    f(:, :, 1) = [-1 0  1; 
                  -1 0  1; 
                  0  0  0];
    f(:, :, 2) = [0  1  0;
                  0  0  -1;
                  0  0  0];
    f(:, :, 3) = [0  1  1;
                  0  0  0;
                  0 -1 -1];
    f(:, :, 4) = [0  0  0;
                  0  0  1;
                  0 -1  0];
    f(:, :, 5) = [0  0  0;
                  -1 0  1;
                  -1 0  1];
    f(:, :, 6) = [0  0  0;
                  -1 0  0;
                  0  1  0];
    f(:, :, 7) = [1  1  0;
                  0  0  0;
                  -1 -1 0];
    f(:, :, 8) = [0  1  0;
                  -1 0  0;
                  0  0  0];
    
    img = rgb2gray(img);
    h = length(img(:, 1));
    w = length(img(1, :));
    % create adj
    adj = zeros(h, w, 8);
    fres = zeros(h, w, 8);
    % find max
    for i = 1:8
        fres(:, :, i) = imfilter(img, f(:, :, i));
    end
    fres = abs(fres);
    maxres = max(max(max(fres)));
    % calc edge
    adj = maxres - fres;
    for i = 1:8
        if mod(i, 2) == 1
            adj(:, :, i) = adj(:, :, i) .* (1/4);
        else
            adj(:, :, i) = adj(:, :, i) .* ((1/2)^(1/2));
        end
    end
