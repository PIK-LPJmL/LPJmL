" ***************************************************************************
" **                                                                       **
" **           f  i  l  e  t  y  p  e  s  .  v  i  m                       **
" **                                                                       **
" **    This files sets for editor vim syntax filetypes for *.conf and     **
" **    and *.par files to c.                                              **
" **    To enable put                                                      **
" **    so $LPJROOT/bin/vim/filetypes.vim                                  **
" **    in $HOME/.vimrc                                                    ** 
" **                                                                       **
" **    written by  Werner von Bloh, PIK Potsdam                           **
" **    Last change: 30.09.2009                                            **
" **                                                                       **
" ***************************************************************************
augroup filetype
        au!
        au! BufRead,BufNewFile *.conf  set filetype=c
        au! BufRead,BufNewFile *.par   set filetype=c
augroup END
