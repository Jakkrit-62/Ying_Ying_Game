from sys import exit
from pygame.locals import *
from gameRole import *
import random
import serial
import odroid_wiringpi as wpi
import time
import pygame

wpi.wiringPiSetup()
# wpi.pinMode(8, 1)  #Rx pin at pin 8 set as input for recieve data from my arduino ohh
# wpi.pinMode(10, 0) #Tx pin at pin 10 set as Output for transfer data to my arduino ^^


arduino_port = '/dev/ttyACM0'
arduino_baudrate = 38400
arduino_serial = serial.Serial(arduino_port, arduino_baudrate)
print("arduino_serial = ",arduino_serial)

pygame.init()
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption('Ying Ying Game')
bullet_sound = pygame.mixer.Sound('resources/sound/enemy1_down.wav')
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

shoot_frequency = 0
enemy_frequency = 0

gameover = False

player_down_index = 16

score = 0

clock = pygame.time.Clock()

running = True
paused = False
sleep_state = False
# wpi.serialFlush(arduino_serial)
arduino_serial.flushInput()
arduino_serial.flushOutput()
start_time = time.time()

while running:
    end_time = time.time()
    elapsed_time = end_time - start_time
    #print(f"รอบลูประยะเวลา: {elapsed_time} วินาที")
    start_time = end_time  # เริ่มจับเวลาใหม่สำหรับรอบถัดไป
    clock.tick(60)
    start_time = time.time()
    arduino_data = ""
    # print(f"wpi.serialDataAvail(arduino_serial) = {wpi.serialDataAvail(arduino_serial)}")
    # while wpi.serialDataAvail(arduino_serial) > 0:
        # arduino_data += chr(wpi.serialGetchar(arduino_serial))
    arduino_data = arduino_serial.readline().decode('latin-1').strip()
    if arduino_data:
        print(f"Received from Arduino: {arduino_data}")
    
    values = arduino_data.split(',')
    print(f"data = {arduino_data}")
    print(f"value = {len(values)}")
    print("len = ",len(values))
    print(f"value99 = {values[0]}")
    if values[0] == '99':
        print("PASS")
        sleep_state = True

        # Display "sleep" on the pygame screen
        font = pygame.font.Font(None, 48)
        sleep_text = font.render('Sleep', True, (255, 255, 255))  # White color
        sleep_text_rect = sleep_text.get_rect()
        sleep_text_rect.centerx = screen.get_rect().centerx
        sleep_text_rect.centery = screen.get_rect().centery
        screen.blit(sleep_text, sleep_text_rect)
        pygame.display.update()
    else:
        sleep_state=False

    if sleep_state:  # Sleep
        continue

    if len(values) == 5:
            print("Loop Read = ",arduino_serial.inWaiting())
            arduino_data_btn_shoot, arduino_data_btn_restart, arduino_data_btn_pause, arduino_data_x, arduino_data_z = map(str, values)
            print(f"Received data from Arduino0: shoot = {arduino_data_btn_shoot}, restart = {arduino_data_btn_restart}, pause = {arduino_data_btn_pause}")
            # wpi.serialFlush(arduino_serial)
            print(f"arduino_data_x = {arduino_data_x} arduino_data_z {arduino_data_z} ")
            arduino_serial.flushInput()
            arduino_serial.flushOutput()

    #button restart
            if arduino_data_btn_restart == '1':
                running,player,gameover, enemies_down, shoot_frequency,enemy_frequency,enemies1, player_down_index, score=reset_game(player)
                paused = False
    #button pause
            if arduino_data_btn_pause == '1':  # Toggle pause when "P" is pressed
                paused = not paused
            if paused:
                continue
    #button shoot
            if arduino_data_btn_shoot == '1':
                print("Shooting Piw Piw\n")
                if shoot_frequency % 2 == 0:
                    bullet_sound.play()
                    player.shoot(bullet_img)
                shoot_frequency += 1
                if shoot_frequency >= 2:
                    shoot_frequency = 0

                # Reset bullets_to_shoot when arduino_data_btn_shoot is not 0
                else:
                    bullets_to_shoot = 0
                    

            #wpi.serialFlush(arduino_serial)
    #ADXL335 Transfer
            player_movement(player,arduino_data_x,arduino_data_z)


    if enemy_frequency % 5 == 0:
        enemy1_pos = [random.randint(0, SCREEN_WIDTH - enemy1_rect.width), 0]
        enemy1 = Enemy(enemy1_img, enemy1_down_imgs, enemy1_pos)
        enemies1.add(enemy1)
    enemy_frequency += 1
    if enemy_frequency >= 100:
        enemy_frequency = 0

    
    for bullet in player.bullets:
        bullet.move()
        if bullet.rect.bottom < 0:
            player.bullets.remove(bullet)

   
    for enemy in enemies1:
        enemy.move()
        
        if pygame.sprite.collide_circle(enemy, player):
            # ถ้าศัตรู (enemy) ชนกับผู้เล่น (player) ในรูปแบบของวงกลม

            # เพิ่มศัตรูที่ชนกับผู้เล่นลงในกลุ่ม enemies_down
            enemies_down.add(enemy)
            
            # นำศัตรูออกจากกลุ่ม enemies1 (ศัตรูทั้งหมด)
            enemies1.remove(enemy)
            
            # ตั้งค่าว่าผู้เล่น (player) ถูกชน
            player.is_hit = True
            
            # เล่นเสียงเมื่อเกม Game Over
            game_over_sound.play()
            
            # ออกจากลูป (loop) ในทันที
            break

        # ถ้าศัตรู (enemy) อยู่ด้านบนของหน้าจอ (มีค่า y มากกว่า SCREEN_HEIGHT)
        # ให้นำศัตรูออกจากกลุ่ม enemies1
        if enemy.rect.top > SCREEN_HEIGHT:
            enemies1.remove(enemy)

    
    enemies1_down = pygame.sprite.groupcollide(enemies1, player.bullets, 1, 1)
    for enemy_down in enemies1_down:
        enemies_down.add(enemy_down)

   
    screen.fill(0)
    screen.blit(background, (0, 0))

    #player.is_hit = False
    if not player.is_hit:
        screen.blit(player.image[player.img_index], player.rect)
       
        player.img_index = shoot_frequency // 8
    else:
        print("โดนชนนนนนน")
        player.img_index = min(player_down_index // 8, len(player.image) - 1)
        screen.blit(player.image[player.img_index], player.rect)
        player_down_index += 1
        if player_down_index > 47:
            gameover = True  #GameOVER interrupt
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
    

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
    #wpi.serialFlush(arduino_serial)

    #มีข้อมูลค้าง ให้เคลียร์
    #if arduino_serial.inWaiting() >=1 :
        #wpi.serialFlush(arduino_serial)
    

    # Check if arduino_data_btn_shoot is 0 and bullets_to_shoot is less than 3

    pygame.display.update()
    time.sleep(0.01)
    print("GameOver = ",gameover)

    while gameover:
        end_time = time.time()
        elapsed_time = end_time - start_time
        print(f"รอบลูประยะเวลา: {elapsed_time} วินาที")
        start_time = end_time  # เริ่มจับเวลาใหม่สำหรับรอบถัดไป
        font = pygame.font.Font(None, 48)
        text = font.render('Score: ' + str(score), True, (255, 0, 0))
        text_rect = text.get_rect()
        text_rect.centerx = screen.get_rect().centerx
        text_rect.centery = screen.get_rect().centery + 24
        screen.blit(game_over, (0, 0))
        screen.blit(text, text_rect)

        arduino_data = arduino_serial.readline().decode().strip()
        values = arduino_data.split(',')

        if len(values) == 5:
            arduino_data_btn_shoot, arduino_data_btn_restart, arduino_data_btn_pause, arduino_data_x, arduino_data_z = map(str, values)
            print(f"Received data from Arduino0: shoot = {arduino_data_btn_shoot}, restart = {arduino_data_btn_restart}, pause = {arduino_data_btn_pause}")
            wpi.serialFlush(arduino_serial)
            

            if arduino_data_btn_restart == '1':
                running,player,gameover, enemies_down, shoot_frequency,enemy_frequency,enemies1, player_down_index, score=reset_game(player)
                break

        pygame.display.update()
        time.sleep(0.01)
