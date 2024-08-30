" *********************************************************************************
" **                                                                             **
" **           f  i  l  e  t  y  p  e  s  .  v  i  m                             **
" **                                                                             **
" **    This files sets for editor vim syntax filetypes for *.cjson files        **
" **    to javascript.                                                           **
" **    To enable put                                                            **
" **    so $LPJROOT/bin/vim/filetypes.vim                                        **
" **    in $HOME/.vimrc                                                          **
" **                                                                             **
" ** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file **
" ** authors, and contributors see AUTHORS file                                  **
" ** This file is part of LPJmL and licensed under GNU AGPL Version 3            **
" ** or later. See LICENSE file or go to http://www.gnu.org/licenses/            **
" ** Contact: https://github.com/PIK-LPJmL/LPJmL                                 **
" **                                                                             **
" *********************************************************************************
augroup filetype
        au!
        au! BufRead,BufNewFile *.cjson set filetype=javascript

augroup END
