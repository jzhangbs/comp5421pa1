function pred = shortest_m(seed_x, seed_y, adj)
    h = length(adj(:, 1, 1));
    w = length(adj(1, :, 1));
    pred = shortest(h, w, seed_x, seed_y, permute(adj, [2 1 3]))';