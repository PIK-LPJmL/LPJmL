Bool bstruct_readtoken(Bstruct bstr,     /**< pointer to restart file */
                       Byte *token_read, /**< token read from file */
                       Byte token,       /**< token expected */
                       const char *name  /**< name of object expected */
                      );