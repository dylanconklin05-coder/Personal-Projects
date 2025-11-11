import copy
import sys
import pygame
import random
import numpy as np

WIDTH = 600
HEIGHT = 600

rows = 3
cols = 3
squareSize = WIDTH // cols

lineWidth = 15
circleWidth = 15
crossWidth = 20

radius = squareSize // 4

OFFSET = 50

# --- COLORS ---

BG_COLOR = (28, 170, 156)
LINE_COLOR = (23, 145, 135)
circleColor = (239, 231, 200)
crossColor = (66, 66, 66)

#  SETUP 

pygame.init()
screen = pygame.display.set_mode( (WIDTH, HEIGHT) )
pygame.display.set_caption('TIC TAC TOE AI')
screen.fill( BG_COLOR )

#  CLASSES

class Board:

    def __init__(self):
        self.squares = np.zeros( (rows, cols) )
        self.emptySquare = self.squares # [squares]
        self.markedSquare = 0

    def final_state(self, show=False):
        '''
            @return 0 if there is no win yet
            @return 1 if player 1 wins
            @return 2 if player 2 wins
        '''

        # vertical wins
        for col in range(cols):
            if self.squares[0][col] == self.squares[1][col] == self.squares[2][col] != 0:
                if show:
                    color = circleColor if self.squares[0][col] == 2 else crossColor
                    iPos = (col * squareSize + squareSize // 2, 20)
                    fPos = (col * squareSize + squareSize // 2, HEIGHT - 20)
                    pygame.draw.line(screen, color, iPos, fPos, lineWidth)
                return self.squares[0][col]

        # horizontal wins
        for row in range(rows):
            if self.squares[row][0] == self.squares[row][1] == self.squares[row][2] != 0:
                if show:
                    color = circleColor if self.squares[row][0] == 2 else crossColor
                    iPos = (20, row * squareSize + squareSize // 2)
                    fPos = (WIDTH - 20, row * squareSize + squareSize // 2)
                    pygame.draw.line(screen, color, iPos, fPos, lineWidth)
                return self.squares[row][0]

        # desc diagonal
        if self.squares[0][0] == self.squares[1][1] == self.squares[2][2] != 0:
            if show:
                color = circleColor if self.squares[1][1] == 2 else crossColor
                iPos = (20, 20)
                fPos = (WIDTH - 20, HEIGHT - 20)
                pygame.draw.line(screen, color, iPos, fPos, crossWidth)
            return self.squares[1][1]

        # asc diagonal
        if self.squares[2][0] == self.squares[1][1] == self.squares[0][2] != 0:
            if show:
                color = circleColor if self.squares[1][1] == 2 else crossColor
                iPos = (20, HEIGHT - 20)
                fPos = (WIDTH - 20, 20)
                pygame.draw.line(screen, color, iPos, fPos, crossWidth)
            return self.squares[1][1]

        # no win yet
        return 0

    def markSquare(self, row, col, player):
        self.squares[row][col] = player
        self.markedSquare += 1

    def empty_sqr(self, row, col):
        return self.squares[row][col] == 0

    def getEmptySquares(self):
        emptySquare = []
        for row in range(rows):
            for col in range(cols):
                if self.empty_sqr(row, col):
                    emptySquare.append( (row, col) )
        
        return emptySquare

    def isfull(self):
        return self.markedSquare == 9

    def isempty(self):
        return self.markedSquare == 0

class AI:

    def __init__(self, level=1, player=2):
        self.level = level
        self.player = player

    #  RANDOM 

    def rnd(self, board):
        emptySquare = board.getEmptySquares()
        idx = random.randrange(0, len(emptySquare))

        return emptySquare[idx] # (row, col)

    #  MINIMAX 

    def minimax(self, board, maximizing):
        
        # terminal case
        case = board.final_state()

        # player 1 wins
        if case == 1:
            return 1, None # eval, move

        # player 2 wins
        if case == 2:
            return -1, None

        # draw
        elif board.isfull():
            return 0, None

        if maximizing:
            max_eval = -100
            best_move = None
            emptySquare = board.getEmptySquares()

            for (row, col) in emptySquare:
                temp_board = copy.deepcopy(board)
                temp_board.mark_sqr(row, col, 1)
                eval = self.minimax(temp_board, False)[0]
                if eval > max_eval:
                    max_eval = eval
                    best_move = (row, col)

            return max_eval, best_move

        elif not maximizing:
            min_eval = 100
            best_move = None
            emptySquare = board.getEmptySquares()

            for (row, col) in emptySquare:
                temp_board = copy.deepcopy(board)
                temp_board.mark_sqr(row, col, self.player)
                eval = self.minimax(temp_board, True)[0]
                if eval < min_eval:
                    min_eval = eval
                    best_move = (row, col)

            return min_eval, best_move

    # MAIN EVAL 

    def eval(self, main_board):
        if self.level == 0:
            # random choice
            eval = 'random'
            move = self.rnd(main_board)
        else:
            # minimax algo choice
            eval, move = self.minimax(main_board, False)

        print(f'AI has chosen to mark the square in pos {move} with an eval of: {eval}')

        return move # row, col

class Game:

    def __init__(self):
        self.board = Board()
        self.ai = AI()
        self.player = 1   #1-cross  #2-circles
        self.gamemode = 'ai' # pvp or ai
        self.running = True
        self.showLines()

    #  DRAW METHODS 

    def showLines(self):
        # bg
        screen.fill( BG_COLOR )

        # vertical
        pygame.draw.line(screen, LINE_COLOR, (squareSize, 0), (squareSize, HEIGHT), lineWidth)
        pygame.draw.line(screen, LINE_COLOR, (WIDTH - squareSize, 0), (WIDTH - squareSize, HEIGHT), lineWidth)

        # horizontal
        pygame.draw.line(screen, LINE_COLOR, (0, squareSize), (WIDTH, squareSize), lineWidth)
        pygame.draw.line(screen, LINE_COLOR, (0, HEIGHT - squareSize), (WIDTH, HEIGHT - squareSize), lineWidth)

    def drawFig(self, row, col):
        if self.player == 1:
            # draw cross
            # desc line
            start_desc = (col * squareSize + OFFSET, row * squareSize + OFFSET)
            end_desc = (col * squareSize + squareSize - OFFSET, row * squareSize + squareSize - OFFSET)
            pygame.draw.line(screen, crossColor, start_desc, end_desc, crossWidth)
            # asc line
            start_asc = (col * squareSize + OFFSET, row * squareSize + squareSize - OFFSET)
            end_asc = (col * squareSize + squareSize - OFFSET, row * squareSize + OFFSET)
            pygame.draw.line(screen, crossColor, start_asc, end_asc, crossWidth)
        
        elif self.player == 2:
            # draw circle
            center = (col * squareSize + squareSize // 2, row * squareSize + squareSize // 2)
            pygame.draw.circle(screen, circleColor, center, radius, circleWidth)

    # --- OTHER METHODS ---

    def makeMove(self, row, col):
        self.board.mark_sqr(row, col, self.player)
        self.drawFig(row, col)
        self.nextTurn()

    def nextTurn(self):
        self.player = self.player % 2 + 1

    def change_gamemode(self):
        self.gamemode = 'ai' if self.gamemode == 'pvp' else 'pvp'

    def isover(self):
        return self.board.final_state(show=True) != 0 or self.board.isfull()

    def reset(self):
        self.__init__()

def main():

    # --- OBJECTS ---

    game = Game()
    board = game.board
    ai = game.ai

    # --- MAINLOOP ---

    while True:
        
        # pygame events
        for event in pygame.event.get():

            # quit event
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

            # keydown event
            if event.type == pygame.KEYDOWN:

                # g-gamemode
                if event.key == pygame.K_g:
                    game.change_gamemode()

                # r-restart
                if event.key == pygame.K_r:
                    game.reset()
                    board = game.board
                    ai = game.ai

                # 0-random ai
                if event.key == pygame.K_0:
                    ai.level = 0
                
                # 1-random ai
                if event.key == pygame.K_1:
                    ai.level = 1

            # click event
            if event.type == pygame.MOUSEBUTTONDOWN:
                pos = event.pos
                row = pos[1] // squareSize
                col = pos[0] // squareSize
                
                # human mark sqr
                if board.empty_sqr(row, col) and game.running:
                    game.makeMove(row, col)

                    if game.isover():
                        game.running = False


        # AI initial call
        if game.gamemode == 'ai' and game.player == ai.player and game.running:

            # update the screen
            pygame.display.update()

            # eval
            row, col = ai.eval(board)
            game.makeMove(row, col)

            if game.isover():
                game.running = False
            
        pygame.display.update()

main()