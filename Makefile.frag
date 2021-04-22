gui: $(SAPI_GUI_PATH)

$(SAPI_GUI_PATH): $(PHP_GLOBAL_OBJS) $(PHP_BINARY_OBJS) $(PHP_GUI_OBJS)
	$(BUILD_GUI)

install-gui: $(SAPI_GUI_PATH)
	@echo "Installing PHP GUI binary:  $(INSTALL_ROOT)$(bindir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(bindir)
	@$(INSTALL) -m 0755 $(SAPI_GUI_PATH) $(INSTALL_ROOT)$(bindir)/$(program_prefix)lsphp$(program_suffix)

