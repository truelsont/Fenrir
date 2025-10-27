extends Node
@onready var game_node = $Node2D/GameNode
@onready var sprite = $Node2D/Sprite2D

var viewport_size = Vector2(800, 600)
var camera_pos = Vector2(0, 0)
var camera_speed = 300.0
var map_size = Vector2(1280, 720)
var zoom_level = 1.0
var min_zoom = 0.5
var max_zoom = 3.0
var zoom_speed = 0.1

func _ready():
	print("GDScript: GameNode created")
	sprite.centered = false
	sprite.position = Vector2.ZERO
	update_texture()
   
func _process(delta):
	if game_node:
		if has_node("Label"):
			$Label.text = "Fenrir - Ticks: %d (Arrows: pan, Wheel: zoom)" % game_node.get_tick_count()
	
	var velocity = Vector2.ZERO
	if Input.is_action_pressed("ui_right"):
		velocity.x += 1
	if Input.is_action_pressed("ui_left"):
		velocity.x -= 1
	if Input.is_action_pressed("ui_down"):
		velocity.y += 1
	if Input.is_action_pressed("ui_up"):
		velocity.y -= 1
	
	if velocity.length() > 0:
		camera_pos += velocity.normalized() * camera_speed * delta
		camera_pos.x = clamp(camera_pos.x, 0, map_size.x - viewport_size.x)
		camera_pos.y = clamp(camera_pos.y, 0, map_size.y - viewport_size.y)
		update_texture()

func update_texture():
	if game_node:
		sprite.texture = game_node.get_world_texture_viewport(
			int(camera_pos.x), 
			int(camera_pos.y),
			int(viewport_size.x),
			int(viewport_size.y)
		)
		sprite.scale = Vector2(zoom_level, zoom_level)
		
func _input(event): 
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_WHEEL_UP and event.pressed:
			zoom_level = clamp(zoom_level + zoom_speed, min_zoom, max_zoom)
			update_texture()
		elif event.button_index == MOUSE_BUTTON_WHEEL_DOWN and event.pressed:
			zoom_level = clamp(zoom_level - zoom_speed, min_zoom, max_zoom)
			update_texture()
	
	if event is InputEventKey and event.pressed and event.keycode == KEY_SPACE:
		if game_node:
			game_node.hello_world()
