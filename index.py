from sys import exit
from pygame.locals import *
from gameRole import *
import random
import serial



# กำหนดพอร์ตที่ Arduino ใช้สำหรับ SoftwareSerial ตาม Xpin, Ypin, Zpin
arduino_port = '/dev/cu.wchusbserial1110'  # แทนด้วยพอร์ตที่ตรงกับการกำหนดใน Arduino
# เริ่มการเชื่อมต่อกับ Arduino ผ่านพอร์ตที่กำหนด
arduino_serial = serial.Serial(arduino_port, 9600)


pygame.init()
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption('Ying Ying Game')


bullet_sound = pygame.mixer.Sound('resources/sound/bullet.wav')
enemy1_down_sound = pygame.mixer.Sound('resources/sound/enemy1_down.wav')
game_over_sound = pygame.mixer.Sound('resources/sound/game_over.wav')
bullet_sound.set_volume(0.3)
enemy1_down_sound.set_volume(0.3)
game_over_sound.set_volume(0.3)
pygame.mixer.music.load('resources/sound/game_music.wav')
pygame.mixer.music.play(-1, 0.0)
pygame.mixer.music.set_volume(0.25)


background = pygame.image.load('resources/image/background.png').convert()
game_over = pygame.image.load('resources/image/gameover.png')

filename = 'resources/image/shoot.png'
plane_img = pygame.image.load(filename)


player_rect = []
player_rect.append(pygame.Rect(0, 99, 102, 126))       
player_rect.append(pygame.Rect(165, 360, 102, 126))
player_rect.append(pygame.Rect(165, 234, 102, 126)) 
player_rect.append(pygame.Rect(330, 624, 102, 126))
player_rect.append(pygame.Rect(330, 498, 102, 126))
player_rect.append(pygame.Rect(432, 624, 102, 126))
player_pos = [200, 600]
player = Player(plane_img, player_rect, player_pos)


bullet_rect = pygame.Rect(1004, 987, 9, 21)
bullet_img = plane_img.subsurface(bullet_rect)


enemy1_rect = pygame.Rect(534, 612, 57, 43)
enemy1_img = plane_img.subsurface(enemy1_rect)
enemy1_down_imgs = []
enemy1_down_imgs.append(plane_img.subsurface(pygame.Rect(267, 347, 57, 43)))
enemy1_down_imgs.append(plane_img.subsurface(pygame.Rect(873, 697, 57, 43)))
enemy1_down_imgs.append(plane_img.subsurface(pygame.Rect(267, 296, 57, 43)))
enemy1_down_imgs.append(plane_img.subsurface(pygame.Rect(930, 697, 57, 43)))

enemies1 = pygame.sprite.Group()


enemies_down = pygame.sprite.Group()

bullets_to_shoot = 0


shoot_frequency = 0
enemy_frequency = 0

player_down_index = 16

score = 0

clock = pygame.time.Clock()

running = True

arduino_serial.flushInput()

while running:

    clock.tick(60)
    
    if enemy_frequency % 20 == 0:
        enemy1_pos = [random.randint(0, SCREEN_WIDTH - enemy1_rect.width), 0]
        enemy1 = Enemy(enemy1_img, enemy1_down_imgs, enemy1_pos)
        enemies1.add(enemy1)
    enemy_frequency += 1
    if enemy_frequency >= 40:
        enemy_frequency = 0

    
    for bullet in player.bullets:
        bullet.move()
        if bullet.rect.bottom < 0:
            player.bullets.remove(bullet)

   
    for enemy in enemies1:
        enemy.move()
        
        if pygame.sprite.collide_circle(enemy, player):
            enemies_down.add(enemy)
            enemies1.remove(enemy)
            player.is_hit = True
            game_over_sound.play()
            break
        if enemy.rect.top > SCREEN_HEIGHT:
            enemies1.remove(enemy)

    
    enemies1_down = pygame.sprite.groupcollide(enemies1, player.bullets, 1, 1)
    for enemy_down in enemies1_down:
        enemies_down.add(enemy_down)

   
    screen.fill(0)
    screen.blit(background, (0, 0))

    player.is_hit = False
    if not player.is_hit:
        screen.blit(player.image[player.img_index], player.rect)
       
        player.img_index = shoot_frequency // 8
    else:
        player.img_index = player_down_index // 8
        screen.blit(player.image[player.img_index], player.rect)
        player_down_index += 1
        if player_down_index > 47:
            running = False

    
    for enemy_down in enemies_down:
        if enemy_down.down_index == 0:
            enemy1_down_sound.play()
        if enemy_down.down_index > 7:
            enemies_down.remove(enemy_down)
            score += 1000
            continue
        screen.blit(enemy_down.down_imgs[enemy_down.down_index // 2], enemy_down.rect)
        enemy_down.down_index += 1

    
    player.bullets.draw(screen)
    enemies1.draw(screen)

    
    score_font = pygame.font.Font(None, 36)
    score_text = score_font.render(str(score), True, (128, 128, 128))
    text_rect = score_text.get_rect()
    text_rect.topleft = [10, 10]
    screen.blit(score_text, text_rect)
    

    
    pygame.display.update()

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
    arduino_serial.flushInput()
    print("FlushBF = ",arduino_serial.inWaiting())
    if arduino_serial.inWaiting() >=1 :
        arduino_serial.flushInput()
        print("Flush = ",arduino_serial.inWaiting())

    arduino_data_btn = int(arduino_serial.readline().decode('latin-1').strip())
    arduino_data_x = int(arduino_serial.readline().decode('latin-1').strip())
    arduino_data_y = int(arduino_serial.readline().decode('latin-1').strip())
    #arduino_data_z = int(arduino_serial.readline().decode('latin-1').strip())
    #arduino_data_btn, arduino_data_x, arduino_data_y, arduino_data_z = map(int, arduino_data_parts)
    print(f"Received data from Arduino: btn = {arduino_data_btn}, x = {arduino_data_x}, y = {arduino_data_y}, z = arduino_data_z")
    #print(f"Received data from Arduino: btn = {type(arduino_data_btn)}, x = {type(arduino_data_x)}, y = {type(arduino_data_y)}, z = arduino_data_z")
    # Adjust player's movement based on Arduino data

    #ADXL335 Transfer
    player_movement(player,arduino_data_x,arduino_data_y)
    
    pygame.display.update()

    # Check if arduino_data_btn is 0 and bullets_to_shoot is less than 3

    if arduino_data_btn == 0:
            if shoot_frequency % 2 == 0:
                bullet_sound.play()
                player.shoot(bullet_img)
            shoot_frequency += 1
            if shoot_frequency >= 2:
                shoot_frequency = 0

            # Reset bullets_to_shoot when arduino_data_btn is not 0
            else:
                bullets_to_shoot = 0
                
            pygame.display.update()
            start_time = pygame.time.get_ticks()
    arduino_serial.flushInput()



font = pygame.font.Font(None, 48)
text = font.render('Score: '+ str(score), True, (255, 0, 0))
text_rect = text.get_rect()
text_rect.centerx = screen.get_rect().centerx
text_rect.centery = screen.get_rect().centery + 24
screen.blit(game_over, (0, 0))
screen.blit(text, text_rect)

while 1:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            arduino_serial.close()
            exit()
    pygame.display.update()
