document.addEventListener('DOMContentLoaded', () => {
  let buttons = document.getElementsByTagName('button');

  for (let i = 0; i < buttons.length; i++) {
    let button = buttons[i];

    button.addEventListener('click', event => {
      event.preventDefault();
      
      let command = button.getAttribute('data-command');

      let xhr = new XMLHttpRequest();
      xhr.open('GET', `/run/${command}`);
      xhr.send();
    });
  }
});
