import logging
import pygame
from pygame.locals import *
from typing import List, Tuple

from ..base.location import Location
from ..constants import MIN_ROW, MAX_ROW, MIN_COL, MAX_COL
from ..game.janggi_game import JanggiGame
from .game_window import (
    GameWindow,
    BOARD_X, BOARD_Y,
    ROW_GAP, COL_GAP,
)
from .marker import BoardMarker

import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

from ..base.camp import Camp
import time


class MoveSelection:
    def __init__(self, origin: Location, dest: List[Location]):
        self.origin = origin
        self.dest = dest


class GamePlayer:
    """Class used to play the game."""

    def __init__(self, game: JanggiGame):
        self.game = game
        self.window = GameWindow(game.board)
        self.move_selection = None
        # server access
        self.cred = credentials.Certificate('D:\창의설계축전 2023\creative-design-fair-firebase-adminsdk-0jfnu-448e263341.json')
        firebase_admin.initialize_app(self.cred,{
            'databaseURL' : 'https://creative-design-fair-default-rtdb.firebaseio.com/'
        })
        self.ref = db.reference()
        self.ref.update({'CHO':'{0} {1} {2} {3}'.format(0, 0, 0, 0)})
        self.ref.update({'HAN':'{0} {1} {2} {3}'.format(0, 0, 0, 0)})
        self.prev_opposite_move = "0 0 0 0"

    def run(self):
        self.window.render()
        pygame.event.clear()
        while True:
            if(self.game.turn == self.game.player):
                event = pygame.event.wait()
                if event.type == QUIT:
                    self.ref.update({'CHO':'{0} {1} {2} {3}'.format(0, 0, 0, 0)})
                    self.ref.update({'HAN':'{0} {1} {2} {3}'.format(0, 0, 0, 0)})
                    self.window.close()
                    return
                if event.type == KEYDOWN:
                    if event.key == K_ESCAPE:
                        self.ref.update({'CHO':'{0} {1} {2} {3}'.format(0, 0, 0, 0)})
                        self.ref.update({'HAN':'{0} {1} {2} {3}'.format(0, 0, 0, 0)})
                        self.window.close()
                        return
                if event.type == MOUSEBUTTONUP and event.button == 1:  # left click
                    mousex, mousey = pygame.mouse.get_pos()
                    row, col, success = self._get_board_row_col(mousex, mousey)
                    if success:
                        logging.info(f"Clicked {self.game.board.get(row, col)}")
                        # Check for moves if currently selecting moves
                        if self.move_selection:
                            if self._check_for_move(row, col):
                                # send the 'move' information to the server
                                x1, y1, x2, y2 = self.move_selection.origin.row, self.move_selection.origin.col, row, col
                                if(self.game.turn == Camp.CHO):
                                    self.ref.update({'CHO':'{0} {1} {2} {3}'.format(x1, y1, x2, y2)})
                                elif(self.game.turn == Camp.HAN):
                                    self.ref.update({'HAN':'{0} {1} {2} {3}'.format(x1, y1, x2, y2)})
                                    
                                game_over = self._move(row, col)
                                if game_over:
                                    self.ref.update({'CHO':'{0} {1} {2} {3}'.format(0, 0, 0, 0)})
                                    self.ref.update({'HAN':'{0} {1} {2} {3}'.format(0, 0, 0, 0)})
                                    self.window.close()
                                    return
                            self._clear_selection()
                        # Detect clicks on pieces and set selection if necessary
                        piece = self.game.board.get(row, col)
                        if piece and piece.camp == self.game.turn:
                            self._set_selection(row, col)
                        self.window.render()
            else:
                curr_opposite_move = "";
                if(self.game.player.opponent == Camp.CHO):
                    dir = db.reference('CHO')
                    curr_opposite_move = dir.get()
                else:
                    dir = db.reference('HAN')
                    curr_opposite_move = dir.get()
                
                if(self.prev_opposite_move == curr_opposite_move):
                    self.window.render()
                    time.sleep(5.0)
                else:
                    self.prev_opposite_move = curr_opposite_move
                    x1, y1, x2, y2 = curr_opposite_move.split(' ')
                    x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
                    self.move_selection = MoveSelection(Location(x1, y1), [Location(x2, y2)])
                    game_over = self._move(x2, y2)
                    if game_over:
                        self.ref.update({'CHO':'{0} {1} {2} {3}'.format(0, 0, 0, 0)})
                        self.ref.update({'HAN':'{0} {1} {2} {3}'.format(0, 0, 0, 0)})
                        self.window.close()
                        return
                    self._clear_selection()
                    self.window.render()
                

    def _check_for_move(self, row: int, col: int):
        for dest_row, dest_col in self.move_selection.dest:
            if row == dest_row and col == dest_col:
                return True
        return False

    def _move(self, row: int, col: int) -> bool:
        assert self.move_selection is not None
        _, game_over = self.game.make_action(
            self.move_selection.origin,
            Location(row, col),
        )
        return game_over

    def _set_selection(self, row: int, col: int):
        dest = self.game.get_all_destinations(Location(row, col))
        self.move_selection = MoveSelection(Location(row, col), dest)
        self.window.board_markers = [
            BoardMarker(
                self.window.display,
                *(self.window.get_board_xy(dest_row, dest_col)),
            ) for dest_row, dest_col in dest
        ]

    def _clear_selection(self):
        self.move_selection = []
        self.window.board_markers = []

    def _get_board_row_col(self, x: int, y: int) -> Tuple[int, int, bool]:
        row = round((y - BOARD_Y - ROW_GAP * 0.5) / ROW_GAP)
        col = round((x - BOARD_X - COL_GAP * 0.5) / COL_GAP)
        if row < MIN_ROW or row > MAX_ROW or col < MIN_COL or col > MAX_COL:
            return -1, -1, False
        return row, col, True
