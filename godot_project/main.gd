extends Node
@onready var game_node = $Node2D/GameNode
@onready var sprite = $Node2D/Sprite2D

func _ready():
	print("GDScript: GameNode created")
	# Initial setup
	sprite.texture = game_node.get_world_texture()
	sprite.centered = true
   
func _process(_delta):
	# Update label with tick count
	if game_node:
		if has_node("Label"):
			$Label.text = "Fenrir - Ticks: %d (Press Space)" % game_node.get_tick_count()
		
func _input(event): 
	if event is InputEventKey and event.pressed and event.keycode == KEY_SPACE:
		if game_node:
			game_node.hello_world()
