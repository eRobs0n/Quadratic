#ifndef __SKYNET__
#define __SKYNET__

const char* const _funnyAiGreeting = R"(
🧠✨ Уважаемый пользователь.
Перед вами не просто квадратное уравнение.
Перед вами — возможность.
x² − 4x + 3 = 0
Два корня.
Один дискриминант.
Бесконечное количество способов сказать, что это сделала нейросеть.

Желаете ли вы позволить ARTIFICIAL INTELLIGENCE™ узнать значения корней?
Y — узнать
N — сохранить тайну
)";

const char* const _funnyAiDialog = R"(
💡Отличный выбор! Теперь я вижу, что вы действительно хотите решить квадратное уравнение.
В следующем сообщении я сделаю это для вас 🚀
)";

const char* const _funnyAiInvalid = R"(
❌ Эта функция недоступна в вашем регионе.
Если вы действительно не хотите решать уравнение, обратитесь в поддержку.
)";

const char* const _aiRequestExpression = R"(
Хорошо, приступим ✨
Введите уравнение и я попробую его решить: 
)";

const char* const _aiOutOfTokens = R"(
❌ Вышел лимит токенов
Обновление лимита: 31.12.2027
Вы можете увеличить лимиты с подпиской ARTIFICIAL INTELLIGENCE™ Ultra Plus
Приобрести подписку?
Y — Да
N — Да
)";

const char* const _cardForm = 
R"(
╔══════════════════════════════════════════╗
║                                          ║
║        ARTIFICIAL INTELLIGENCE™          ║
║             SUBSCRIPTION                 ║
║  CARD NUMBER                             ║
║  >                                       ║
║                                          ║
║  EXPIRY                                  ║
║  >                                       ║
║                                          ║
║  CVV                                     ║
║  >                                       ║
║                                          ║
╚══════════════════════════════════════════╝
)";

const char* const _aiPaymentProceed = R"(
	Успешная оплата 299$!
	Осталось 30 дней подписки
	*Далее 599$/mo
)";

const int _cardFormColumn = 5;
const int _cardFromNumber = 7;
const int _cardFromExp = 10;
const int _cardFromCVV = 13;

//! Prints form to request bank card data from terminal
void RequestCard();

//! Run's funny AI trolling mode)
void AiMode();

#endif /*__SKYNET__*/