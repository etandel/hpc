-- Code based on tutorial at http://ai-depot.com/Articles/51/TSP.html

import random, sqrt, floor from math
import remove from table

GRID_SIZE     = 255 -- grid goes from 0 to 255 in both axes
NUM_VERTEXES  = 256 -- number of towns / number of genes
POP_SIZE      = 1000 -- size of population must be even
CROSS_PROB    = 0.9 -- probability of crossover occuring
MUTATION_PROB = 0.01 --probabily of mutation occuring
-- number of generations not improved that stop execution
STAG_COUNT    = 500
-- number proportional to the severeness of fitness evaluation
FIT_PRECISION = 10000000

range = (max) ->
    --returns an iterator that returns numbers from 1 to max in sequence
    last = 0
    (using last) ->
        last += 1
        last <= max and last or nil


random_point = ->
    {random(0, GRID_SIZE), random(0, GRID_SIZE)}

gen_vertexes = ->
    repeated = setmetatable {} ,
        __call: (p) =>
            x, y = p[1], p[2]
            if not self[x]
                self[x] =
                    [y]: true
                return false
            elseif not self[x][y]
                self[x][y] = true
                return false
            else
                return true
            
    return for i = 1, NUM_VERTEXES
        new_p = random_point!
        while repeated(new_p)
            new_p = random_point!
        new_p


class Pool
    new: =>
        for i = 1, NUM_VERTEXES
            self[i] = i

    remove: (gene) =>
        remove self, gene

    get_random_gene: =>
        index = random #self
        return self[index]
        

gen_subject = () ->
    pool = Pool!
    tour  = {}
    for i = 1, NUM_VERTEXES
        gene = pool\get_random_gene!
        pool\remove gene
        tour[#tour+1] = gene
    return tour

distance = (p1, p2) ->
    sqrt (p2[1] - p1[1])^2 , (p2[1] - p1[2])^2

tour_length = (subj) ->
    total_len = 0
    for i = 1, NUM_VERTEXES - 1
        gene, next_gene = subj[i] , subj[i+1]
        total_len += distance vertexes[next_gene], vertexes[gene]
    --back to initial town
    gene, next_gene = subj[1] , subj[NUM_VERTEXES]
    total_len += distance vertexes[next_gene], vertexes[gene]
    return total_len
    
    
fitness = (subj) ->
    -- number usually begins on the fifth decimal
    return FIT_PRECISION / tour_length(subj)

crossover = (parent_a, parent_b, cross_point) ->
    child     = {}
    pool      = Pool!

    -- first half of genes is the same as parent_x
    for i = 1, cross_point
        gene = parent_a[i]
        child[i] = gene
        pool\remove(gene)
        
    -- first pass completes with other parent, but leaves holes when repeated
    for i = cross_point+1, NUM_VERTEXES
        gene = parent_b[i]
        if pool[gene]
            pool\remove gene
        else
            gene = nil
        child[i] = gene

    -- second pass fills gaps with random non-repeated genes
    for i = cross_point+1, NUM_VERTEXES
        if not child[i]
            child[i] = pool\get_random_gene!
    return child

mutate = (subj) ->
    -- swaps two random different genes
    gene_a = random NUM_VERTEXES
    gene_b = gene_a
    while gene_b == gene_a
        gene_b = random NUM_VERTEXES

    subj[gene_a], subj[gene_b] = subj[gene_b], subj[gene_a]

reproduce = (parent_a, parent_b) ->
    must_cross = random! <= CROSS_PROB and true or false
    must_mutate = random! <= MUTATION_PROB and true or false

    cross_point = random NUM_VERTEXES - 1
    child_a = must_cross and crossover(parent_a, parent_b, cross_point) or parent_a
    child_b = must_cross and crossover(parent_b, parent_a, cross_point) or parent_b

    if must_mutate
        mutate child_a
        mutate child_b

    return child_a, child_b

--get_random_parents = ->
--    prob_pool = {}
--    for subj_index, fit_score in ipairs fitness_score
--        for i = 1, floor(fit_score * 1000)
--            prob_pool[#prob_pool+1] = subj_index

class Population
    new: (init_pop=nil) =>
        @max_fitness       = 0
        @fittest           = 0
        @_fit_map          = {}
        @_fit_prob_pool    = {}

        if init_pop
            self\add_subject(subj) for i, subj in ipairs init_pop

    add_subject: (subj) =>
        self[#self+1] = subj
        fit = fitness(subj)
        @_fit_map[subj] = fit
        if fit > @max_fitness
            @max_fitness = fit
            @fittest     = subj
        -- how much "space" this subject deserves on the rep. pool
        for i=1, fit
            @_fit_prob_pool[#@_fit_prob_pool+1] = subj

    get_random_parents: =>
        max_index = #@_fit_prob_pool
        parent_a = @_fit_prob_pool[random max_index]
        parent_b = parent_a
        while parent_b == parent_a
            parent_b = @_fit_prob_pool[random max_index]
        return parent_a, parent_b


-- Beginning of program

math.randomseed os.time!

export vertexes = gen_vertexes!
max_fitness = 0
fittest = 0
stag_count = 0

-- main loop
main = (using max_fitness, fittest, stag_count)->
    old_pop = Population [gen_subject! for i in range NUM_VERTEXES]

    while true
        new_pop = Population!

        -- reproduce (including crossovers and mutations)
        for i = 1, POP_SIZE/2
            parent_a, parent_b = old_pop\get_random_parents!
            child_a, child_b = reproduce(parent_a, parent_b)
            new_pop\add_subject child_a
            new_pop\add_subject child_b

        if new_pop.max_fitness > max_fitness --or new_pop.max_fitness < max_fitness
            max_fitness = new_pop.max_fitness
            fittest = new_pop.fittest
            stag_count = 0
        else
            stag_count += 1

        io.write ('Max fitness: %.17f\n')\format(max_fitness)

        if stag_count == STAG_COUNT
            break

    io.write string.format 'Length: %.17f\n', tour_length(fittest)
    -- print vertexes[gene][1], vertexes[gene][2] for gene in *fittest
    
main!
