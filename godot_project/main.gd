extends Node

var game_node: GameNode

func _ready():
	# Create C++ GameNode
	game_node = GameNode.new()
	add_child(game_node)
	print("GDScript: GameNode created")

func _process(_delta):
	# Update label with tick count
	if game_node:
		$Label.text = "Fenrir - Ticks: %d (Press Space)" % game_node.get_tick_count()

func _input(event):
	if event is InputEventKey and event.pressed and event.keycode == KEY_SPACE:
		if game_node:
			game_node.hello_world()
