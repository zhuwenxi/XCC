def db_log(debug=True, *args, **kwargs):
	if debug:
		print(*args, **kwargs)