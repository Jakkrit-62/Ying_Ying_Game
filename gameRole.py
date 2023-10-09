import pygame

#Thinkspeak
import urllib
import requests

SCREEN_WIDTH = 480
SCREEN_HEIGHT = 800

TYPE_SMALL = 1
TYPE_MIDDLE = 2
TYPE_BIG = 3


class Bullet(pygame.sprite.Sprite):
    def __init__(self, bullet_img, init_pos):
        pygame.sprite.Sprite.__init__(self)
        self.image = bullet_img
        self.rect = self.image.get_rect()
        self.rect.midbottom = init_pos
        self.speed = 20

    def move(self):
        self.rect.top -= self.speed


class Player(pygame.sprite.Sprite):
    def __init__(self, plane_img, player_rect, init_pos):
        pygame.sprite.Sprite.__init__(self)
        self.image = []                             
        for i in range(len(player_rect)):
            self.image.append(plane_img.subsurface(player_rect[i]).convert_alpha())
        self.rect = player_rect[0]                    
        self.rect.topleft = init_pos                   
        self.speed = 20                        
        self.bullets = pygame.sprite.Group()           
        self.img_index = 0                             
        self.is_hit = False                             

    def shoot(self, bullet_img):
        bullet = Bullet(bullet_img, self.rect.midtop)
        self.bullets.add(bullet)

    def moveUp(self):
        if self.rect.top <= 0:
            self.rect.top = 0
        else:
            self.rect.top -= self.speed

    def moveDown(self):
        if self.rect.top >= SCREEN_HEIGHT - self.rect.height:
            self.rect.top = SCREEN_HEIGHT - self.rect.height
        else:
            self.rect.top += self.speed

    def moveLeft(self):
        if self.rect.left <= 0:
            self.rect.left = 0
        else:
            self.rect.left -= self.speed

    def moveRight(self):
        if self.rect.left >= SCREEN_WIDTH - self.rect.width:
            self.rect.left = SCREEN_WIDTH - self.rect.width
        else:
            self.rect.left += self.speed

class Enemy(pygame.sprite.Sprite):
    def __init__(self, enemy_img, enemy_down_imgs, init_pos):
       pygame.sprite.Sprite.__init__(self)
       self.image = enemy_img
       self.rect = self.image.get_rect()
       self.rect.topleft = init_pos
       self.down_imgs = enemy_down_imgs
       self.speed = 5
       self.down_index = 0

    def move(self):
        self.rect.top += self.speed

def player_movement(player,arduino_data_x, arduino_data_z):
  
  if arduino_data_x == '0':
    player.moveDown()
  elif arduino_data_x == '1':
    player.moveUp()

  if arduino_data_z == '1':
    player.moveRight()
  elif arduino_data_z == '0':
    player.moveLeft()

 
 
def reset_game(player):

    #global gameover, enemies_down, shoot_frequency, enemy_frequency, player_down_index, score
    running = True
    gameover = False
    player.is_hit = False
    player.img_index = 0
    enemies1 = pygame.sprite.Group()
    enemies_down = pygame.sprite.Group()
    player.bullets = pygame.sprite.Group()
    shoot_frequency = 0
    enemy_frequency = 0
    player_down_index = 16
    score = 0

    # reset the player's position
    player.rect.x = 200
    player.rect.y = 600

    # reset the enemy's position
    for enemy in enemies1:
        enemies1.remove(enemy)

    for bullet in player.bullets:
        player.bullets.remove(bullet)

    return running,player,gameover, enemies_down, shoot_frequency, enemy_frequency,enemies1, player_down_index, score


def read_highscore():
    try:
        with open('highscore.txt', 'r') as file:
            highscore = int(file.read())
    except FileNotFoundError:
        highscore = 0
    return highscore
def write_highscore(highscore):
    with open('highscore.txt', 'w') as file:
        file.write(str(highscore))
round_scores = []

# ฟังก์ชันสำหรับเพิ่มคะแนนรอบลงในรายการ
def add_round_score(score):
    round_scores.append(score)

# ฟังก์ชันสำหรับเขียนรายการคะแนนรอบทั้งหมดในไฟล์
def write_round_scores_to_file():
    with open('scores.txt', 'w') as file:
        for score in round_scores:
            file.write(str(score) + '\n')


#THINKSPEAK
def update_thingspeak(score, highscore):
    api_key = 'QEATA5NV3U4JPVYK'  # API
    base_url = 'https://api.thingspeak.com/update?'

    # Construct the URL with the data to be sent
    url_params = {
        'api_key': api_key,
        'field1': score,
        'field2': highscore
    }
    url = base_url + urllib.parse.urlencode(url_params)

    try:
        # Use requests library to make a GET request to ThingSpeak
        response = requests.get(url)
        if response.status_code == 200:
            print("Data sent to ThingSpeak successfully!")
        else:
            print("Failed to send data to ThingSpeak. Status code:", response.status_code)
    except Exception as e:
        print("Error:", e)