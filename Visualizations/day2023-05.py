from dataclasses import dataclass, astuple
import sys
from typing import List, Tuple, Set, Iterator, Iterable
import pygame
from pygame import draw, Rect
from colorsys import hsv_to_rgb
from time import time_ns, sleep
from contextlib import contextmanager
import itertools as it

TRUE_SCALE = 2

WINDOW_WIDTH = 1920 * TRUE_SCALE
WINDOW_HEIGHT = 1080 * TRUE_SCALE

BG_COLOR = (0,) * 3
OUTLINE_COLOR = (int(255 / 2),) * 3
SEED_COLOR = (0, 255, 255)
SEED_RANGE_COLOR = (128, 128, 128)
BOX_COLORS = []

SPACING = 0
PIPE_HEIGHT_FACTOR = 3
PER_LINE_SCALE = 2 + PIPE_HEIGHT_FACTOR + SPACING

SCALE_Y = 0
SCALE_X = 0
SEED_SIZE = 7 * TRUE_SCALE

LINE_WIDTH = 0 * TRUE_SCALE
RECT_WITH = 1 * TRUE_SCALE
TRAVEL_WIDTH = 4 * TRUE_SCALE

ANIMATE = True

SPEED = 1

FRAME_RATE = 60
BILLION = 1_000_000_000
SLEEP_DURATION = BILLION / FRAME_RATE
LAST_SLEEP = time_ns()


def sleep_until():
    handleQuit()
    if not ANIMATE:
        return
    pygame.display.update()
    global LAST_SLEEP
    to_sleep = LAST_SLEEP + SLEEP_DURATION
    curr = time_ns()
    if curr < to_sleep:
        sleep((to_sleep - curr) / BILLION)
    LAST_SLEEP = time_ns()


@contextmanager
def temp_values(**kwargs):
    old_values = {}
    for key, new in kwargs.items():
        val = globals()[key]
        old_values[key] = val
        globals()[key] = new
    yield
    for key, old in old_values.items():
        globals()[key] = old


def generate_box_colors(size):
    step = 1 / size
    for i in range(size):
        rgb = hsv_to_rgb(i * step, 1.0, 1.0)
        yield tuple(map(lambda x: int(x * 255 / 2), rgb))


def set_size_dependent_config(largest_num, num_of_steps, max_maps):
    global SCALE_X, SCALE_Y, BOX_COLORS
    SCALE_X = WINDOW_WIDTH / largest_num
    SCALE_Y = WINDOW_HEIGHT / (num_of_steps * (SPACING + 2 + PIPE_HEIGHT_FACTOR) + 1)
    BOX_COLORS = list(generate_box_colors(max_maps))


pygame.init()
pygame.display.set_caption("2023 Day 05")
canvas = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT), display=0)


def precalc_midpoints(denom):
    parts = FRAME_RATE / denom
    interval = 1 / parts
    for i in range(1, int(parts) + 1):
        yield interval * i


QUARTER_SECOND_MIDPOINTS = list(precalc_midpoints(8))
HALF_SECOND_MIDPOINTS = list(precalc_midpoints(4))
FULL_SECOND_MIDPOINTS = list(precalc_midpoints(2))
TWO_SECOND_MIDPOINTS = list(precalc_midpoints(1))

print(len(QUARTER_SECOND_MIDPOINTS))


def line_midpoint(start, end, midpoint):
    startx, starty = start
    endx, endy = end
    return (
        startx + (endx - startx) * midpoint,
        starty + (endy - starty) * midpoint,
    )


"""
        ----------    < (i - 1) * PER_LINE_SCALE * SCALE_Y
        |        |
V0      ---------- 

    #### SPACING ####
        H1       H2
V1      ----------
        |        |
V2      ---------- 
       /         /
      /         /
V3   ----------        < i * PER_LINE_SCALE * SCALE_Y
     |        |
V4   ----------
     H3       H4
"""


def Vertical_Coords(index):
    V3 = index * PER_LINE_SCALE * SCALE_Y
    V4 = V3 + SCALE_Y
    V2 = V3 - PIPE_HEIGHT_FACTOR * SCALE_Y
    V1 = V2 - SCALE_Y
    V0 = V1 - SPACING
    return V1, V2, V3, V4


def Horizontal_Coords(dstart, sstart, size):
    H1 = sstart * SCALE_X
    H2 = (sstart + size) * SCALE_X
    H3 = dstart * SCALE_X
    H4 = (dstart + size) * SCALE_X
    return H1, H2, H3, H4


@dataclass
class Seed:
    data: List[int]


@dataclass
class Seed2:
    data: List[Tuple[int, int]]

    def in_range(self, seed):
        return any(0 <= (seed - start) < len for start, len in self.data)

    def in_range2(self, seed1, seed2):
        return any((0 <= (seed1 - start) < len) and (0 <= (seed2 - start) < len) for start, len in self.data)


@dataclass
class IntMap:
    dest_start: int
    source_start: int
    distance: int

    def in_range(self, seed):
        dist = seed - self.source_start
        return 0 <= dist < self.distance

    def convert(self, seed):
        dist = seed - self.source_start
        return self.dest_start + dist

    def in_range_rev(self, seed):
        dist = seed - self.dest_start
        return 0 <= dist < self.distance

    def convert_rev(self, seed):
        dist = seed - self.dest_start
        return self.source_start + dist


@dataclass
class IntMaps:
    maps: List[IntMap]

    def convert(self, seed):
        for map in self.maps:
            if map.in_range(seed):
                return map.convert(seed)
        return seed

    def convert_rev(self, vec: Iterable[int]):
        marks = list(vec) + [mapp.dest_start for mapp in self.maps]
        mapping = {}
        for mark in marks:
            for mapp in self.maps:
                if mapp.in_range_rev(mark):
                    mapping[mark] = mapp.convert_rev(mark)
            if mark not in mapping:
                mapping[mark] = mark
        return mapping


@dataclass
class Pipeline:
    pipe: List[IntMaps]

    def run_through_pipeline(self, seeds: Seed):
        seeds_data = seeds.data
        # SPACING
        if SPACING:
            assert (False, "ADD SPACING CODE")

        for index, pipe in enumerate(self.pipe, start=1):
            V1, V2, V3, V4 = Vertical_Coords(index)

            # SOURCE RANGES
            bounds = [((seed * SCALE_X, V1), (seed * SCALE_X, V2)) for seed in seeds_data]
            for mid in it.chain(HALF_SECOND_MIDPOINTS[::SPEED], (HALF_SECOND_MIDPOINTS[-1],)):
                for start, end in bounds:
                    midpoint = line_midpoint(start, end, mid)
                    draw.line(canvas, SEED_COLOR, start, midpoint, TRAVEL_WIDTH)
                sleep_until()

            # TRANSFORMATION
            new_seeds = list(map(lambda seed: pipe.convert(seed), seeds_data))
            bounds = [((old * SCALE_X, V2), (new * SCALE_X, V3)) for old, new in zip(seeds_data, new_seeds)]
            for mid in it.chain(FULL_SECOND_MIDPOINTS[::SPEED], (FULL_SECOND_MIDPOINTS[-1],)):
                for start, end in bounds:
                    midpoint = line_midpoint(start, end, mid)
                    draw.line(canvas, SEED_COLOR, start, midpoint, TRAVEL_WIDTH)
                sleep_until()

            # DESTINATION RANGES
            bounds = [((seed * SCALE_X, V3), (seed * SCALE_X, V4)) for seed in new_seeds]
            for mid in it.chain(HALF_SECOND_MIDPOINTS[::SPEED], (HALF_SECOND_MIDPOINTS[-1],)):
                for start, end in bounds:
                    midpoint = line_midpoint(start, end, mid)
                    draw.line(canvas, SEED_COLOR, start, midpoint, TRAVEL_WIDTH)
                sleep_until()

            seeds_data = new_seeds
        return list(zip(seeds_data, seeds.data))

    def run_through_pipeline_rev(self):
        vec = set()
        for index, mapp in zip(range(len(self.pipe), 0, -1), reversed(self.pipe)):
            mapping = mapp.convert_rev(vec)
            V1, V2, V3, V4 = Vertical_Coords(index)

            # DESTINATION
            bounds = [((seed * SCALE_X, V4), (seed * SCALE_X, V3)) for seed in mapping]
            for mid in it.chain(HALF_SECOND_MIDPOINTS[::SPEED], (HALF_SECOND_MIDPOINTS[-1],)):
                for start, end in bounds:
                    midpoint = line_midpoint(start, end, mid)
                    draw.line(canvas, SEED_COLOR, start, midpoint, TRAVEL_WIDTH)
                sleep_until()

            # TRANSFORMATION
            bounds = [((old * SCALE_X, V3), (new * SCALE_X, V2)) for old, new in mapping.items()]
            for mid in it.chain(FULL_SECOND_MIDPOINTS[::SPEED], (FULL_SECOND_MIDPOINTS[-1],)):
                for start, end in bounds:
                    midpoint = line_midpoint(start, end, mid)
                    draw.line(canvas, SEED_COLOR, start, midpoint, TRAVEL_WIDTH)
                sleep_until()

            # SOURCE
            bounds = [((seed * SCALE_X, V2), (seed * SCALE_X, V1)) for seed in mapping.values()]
            for mid in it.chain(HALF_SECOND_MIDPOINTS[::SPEED], (HALF_SECOND_MIDPOINTS[-1],)):
                for start, end in bounds:
                    midpoint = line_midpoint(start, end, mid)
                    draw.line(canvas, SEED_COLOR, start, midpoint, TRAVEL_WIDTH)
                sleep_until()

            vec = set(mapping.values())
        return vec


def parseSeed(line: str):
    nums_str = line[7:-1]
    nums = list(map(int, nums_str.split(' ')))
    return Seed(nums)


def pairwise(iterable):
    """s -> (s0, s1), (s2, s3), (s4, s5), ..."""
    a = iter(iterable)
    return zip(a, a)


def parseSeed2(line: str):
    nums_str = line[7:-1]
    nums = list(pairwise(map(int, nums_str.split(' '))))
    return Seed2(nums)


def parseIntMap(line: str):
    return IntMap(*tuple(map(int, line.rstrip().split(' '))))


def parseIntMaps(lines: Iterator[str]):
    toret = IntMaps([])
    for line in map(str.rstrip, lines):
        if not line:
            return toret
        toret.maps.append(parseIntMap(line))
    return toret


def parsePipeline(lines: Iterator[str]):
    toret = Pipeline([])
    for line in lines:
        if not line:
            return toret
        toret.pipe.append(parseIntMaps(lines))
    return toret


def handleQuit():
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            exit(0)


def printSeeds(seeds: Seed):
    for seed in seeds.data:
        draw.circle(canvas, SEED_COLOR, (seed * SCALE_X, SCALE_Y / 2), SEED_SIZE)


def printSeeds2(seeds: Seed2):
    for seed, size in seeds.data:
        draw.rect(canvas, SEED_RANGE_COLOR, Rect(seed * SCALE_X, 0, size * SCALE_X, SCALE_Y))
        if RECT_WITH:
            draw.rect(canvas, OUTLINE_COLOR, Rect(seed * SCALE_X, 0, size * SCALE_X, SCALE_Y), width=RECT_WITH)


def drawInitialSeed(seeds: Seed, rev=False):
    bounds = [((seed * SCALE_X, SCALE_Y / 2), (seed * SCALE_X, SCALE_Y)) for seed in seeds.data]
    for mid in it.chain(FULL_SECOND_MIDPOINTS[::SPEED], (FULL_SECOND_MIDPOINTS[-1],)):
        for start, end in bounds:
            if rev:
                end, start = start, end
            midpoint = line_midpoint(start, end, mid)
            draw.line(canvas, SEED_COLOR, start, midpoint, TRAVEL_WIDTH)
        sleep_until()


def printPipeline(pipeline: Pipeline):
    """
            ----------    < (i - 1) * PER_LINE_SCALE * SCALE_Y
            |        |
            ---------- 
    
        #### SPACING ####
            H1       H2
    V1      ----------
            |        |
    V2      ---------- 
           /         /
          /         /
    V3   ----------        < i * PER_LINE_SCALE * SCALE_Y
         |        |
    V4   ----------
         H3       H4
    """
    for i, pipe in enumerate(pipeline.pipe, start=1):
        V3 = i * PER_LINE_SCALE * SCALE_Y
        V4 = V3 + SCALE_Y
        V2 = V3 - PIPE_HEIGHT_FACTOR * SCALE_Y
        V1 = V2 - SCALE_Y
        for j, (dstart, sstart, size) in enumerate(map(astuple, pipe.maps)):
            H1 = sstart * SCALE_X
            H2 = (sstart + size) * SCALE_X
            H3 = dstart * SCALE_X
            H4 = (dstart + size) * SCALE_X

            draw.line(canvas, BOX_COLORS[j],
                      (H3, V3),
                      (H1, V2),
                      LINE_WIDTH)
            draw.line(canvas, BOX_COLORS[j],
                      (H4, V3),
                      (H2, V2),
                      LINE_WIDTH)

            draw.rect(canvas, BOX_COLORS[j], Rect(H1, V1, size * SCALE_X, SCALE_Y))
            draw.rect(canvas, BOX_COLORS[j], Rect(H3, V3, size * SCALE_X, SCALE_Y))
            if (RECT_WITH):
                draw.rect(canvas, OUTLINE_COLOR, Rect(H1, V1, size * SCALE_X, SCALE_Y), width=RECT_WITH)
                draw.rect(canvas, OUTLINE_COLOR, Rect(H3, V3, size * SCALE_X, SCALE_Y), width=RECT_WITH)


def puzzle1(filename):
    with open(filename) as f:
        seeds = parseSeed(next(f))
        next(f)
        pipeline = parsePipeline(f)

    max_num = max(dest + size for pipe in pipeline.pipe for dest, start, size in map(astuple, pipe.maps))
    max_maps = max(len(mapp.maps) for mapp in pipeline.pipe)
    set_size_dependent_config(max_num, len(pipeline.pipe), max_maps)

    canvas.fill(BG_COLOR)
    printPipeline(pipeline)
    printSeeds(seeds)
    pygame.display.update()
    input("Waiting for input")
    drawInitialSeed(seeds)
    locations = pipeline.run_through_pipeline(seeds)
    min_loc, min_seed = min(locations)

    minseed = Seed([min_seed])
    with temp_values(SEED_COLOR=tuple(x / 2 for x in SEED_COLOR), ANIMATE=False):
        canvas.fill(BG_COLOR)
        printPipeline(pipeline)
        printSeeds(seeds)
        drawInitialSeed(seeds)
        pipeline.run_through_pipeline(seeds)
    with temp_values(SEED_COLOR=(128, 255, 128), TRAVEL_WIDTH=10*TRUE_SCALE, SEED_SIZE=10*TRUE_SCALE, SPEED=8):
        printSeeds(minseed)
        drawInitialSeed(minseed)
        pipeline.run_through_pipeline(minseed)

    sleep(3)

    print(min_loc)


def puzzle2(filename):
    with open(filename) as f:
        seeds = parseSeed2(next(f))
        next(f)
        pipeline = parsePipeline(f)

    max_num = max(dest + size for pipe in pipeline.pipe for dest, start, size in map(astuple, pipe.maps))
    max_maps = max(len(mapp.maps) for mapp in pipeline.pipe)
    set_size_dependent_config(max_num, len(pipeline.pipe), max_maps)

    canvas.fill(BG_COLOR)
    printPipeline(pipeline)
    printSeeds2(seeds)
    pygame.display.update()

    sleep(3)

    with temp_values(SEED_COLOR=(255, 128, 128), TRAVEL_WIDTH=TRUE_SCALE * 2, SEED_SIZE=TRUE_SCALE * 5):
        boundaries = pipeline.run_through_pipeline_rev()
        mock_seeds = Seed(list(boundaries))
        drawInitialSeed(mock_seeds, rev=True)
        printSeeds(mock_seeds)
        pygame.display.update()

    sleep(3)
    mock_seeds.data.sort()
    widdest_bounary = max(
        (b - a, a) for a, b in it.pairwise(mock_seeds.data))
    with temp_values(SEED_COLOR=tuple(x / 2 for x in SEED_COLOR), ANIMATE=False):
        canvas.fill(BG_COLOR)
        printPipeline(pipeline)
        printSeeds2(seeds)
        printSeeds(mock_seeds)
    with temp_values(SEED_COLOR=(128, 255, 128), TRAVEL_WIDTH=TRUE_SCALE * 5, SEED_SIZE=TRUE_SCALE * 10, ANIMATE=False):
        binded_seeds = Seed([widdest_bounary[1], widdest_bounary[0] + widdest_bounary[1] - 1])
        printSeeds2(seeds)
        printSeeds(binded_seeds)
        drawInitialSeed(binded_seeds)
        pipeline.run_through_pipeline(binded_seeds)

    in_bounds_seeds = Seed([seed for seed in mock_seeds.data if seeds.in_range(seed)])
    in_bounds_seeds.data.extend(seed[0] for seed in seeds.data)

    canvas.fill(BG_COLOR)
    printPipeline(pipeline)
    printSeeds2(seeds)
    printSeeds(in_bounds_seeds)
    pygame.display.update()
    sleep(3)
    drawInitialSeed(in_bounds_seeds)
    locations = pipeline.run_through_pipeline(in_bounds_seeds)
    min_loc, min_seed = min(locations)

    minseed = Seed([min_seed])
    with temp_values(SEED_COLOR=tuple(x / 2 for x in SEED_COLOR), ANIMATE=False):
        canvas.fill(BG_COLOR)
        printPipeline(pipeline)
        printSeeds2(seeds)
        printSeeds(in_bounds_seeds)
        drawInitialSeed(in_bounds_seeds)
        pipeline.run_through_pipeline(in_bounds_seeds)
    with temp_values(SEED_COLOR=(128, 255, 128), TRAVEL_WIDTH=TRUE_SCALE * 10, SEED_SIZE=TRUE_SCALE * 10, SPEED=8):
        printSeeds(minseed)
        drawInitialSeed(minseed)
        pipeline.run_through_pipeline(minseed)

    # boundaries = pipeline.run_through_pipeline_rev()
    # boundaries.update(a for a, b in seeds.data)
    # res = min(pipeline.run_through_pipeline(seed) for seed in boundaries if seeds.in_range(seed))
    # print(res)


def main():
    a = sys.argv[-1] == "sample"
    filename = f"../puzzles/2023/inputs/day05_{'sample_' if a else ''}input.txt"
    puzzle1(filename)
    puzzle2(filename)

    while True:
        handleQuit()
        sleep(2)


if __name__ == '__main__':
    main()
